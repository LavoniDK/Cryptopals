#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "ascii.h"
#include "linalg.h"

using BYTE = unsigned char;
using json = nlohmann::ordered_json;

json profile_for(const std::string& email_input);
json parse_kv(const std::string& s);
std::string encode_profile(const json& profile);
std::vector<std::string> split(const std::string& s, char delim);
std::vector<BYTE> encrypt_profile(const json& profile, const std::vector<BYTE>& key);
json decrypt_profile(const std::vector<BYTE>& ct, const std::vector<BYTE>& key);
std::vector<BYTE> rand_bytes(const size_t len);

int main() {
    
    std::vector<BYTE> secret_key = rand_bytes(16);
    const size_t block_size = 16; // Assuming AES-ECB-128, therefor hardcoding some lengths of payload 
   
    // choose plaintext "email=foooooooo.admin<padding>com&uid=10&role=user<padding>"
    std::string mail_name = "foooooooo"; // arbitrary characters of length blocksize - "email".size() - ".".size()
    std::string admin = "admin";
    std::string pkcs7_pad(11, static_cast<char>(0x0b));
    std::string admin_email = mail_name + "." + admin + pkcs7_pad + "com"; 
    json admin_profile = profile_for(admin_email);

    std::cout << "Profile and its encoding: " << std::endl;
    std::cout << admin_profile.dump(4) << std::endl;
    std::string admin_pt = encode_profile(admin_profile);
    std::cout << admin_pt << std::endl;
    std::cout << '\n' << "===================================" << "\n\n";

    // encrypt profile
    std::vector<BYTE> ct_admin = encrypt_profile(admin_profile, secret_key);
    std::vector<std::vector<BYTE>> blocks = split_vector(ct_admin, block_size);
    
    // choose ciphertexts
    std::vector<std::vector<BYTE>> forged_ct;
    forged_ct.push_back(blocks[0]); // first block contains email=foooooooo. 
    forged_ct.push_back(blocks[2]); // third block contains com&uid=10&role=
    forged_ct.push_back(blocks[1]); // second block contains admin+padding, exchange for the original last block of user+padding

    // decrypt and parse 
    json parsed = decrypt_profile(flatten_matrix(forged_ct), secret_key);
    std::cout << "forged token: " << encode_profile(parsed) << std::endl;


    return 0;
}

json profile_for(const std::string& email_input) {
    // strip metacharacters (eat & and =)
    std::string clean;
    clean.reserve(email_input.size());
    for (char c : email_input) {
        if (c != '&' && c != '=') clean.push_back(c);
    }

    json profile;
    profile["email"] = clean;
    profile["uid"]   = "10";
    profile["role"]  = "user";
    return profile;
}

json parse_kv(const std::string& s) {
    json obj;
    std::stringstream ss(s);
    std::string part;

    while (std::getline(ss, part, '&')) {
        auto pos = part.find('=');
        if (pos == std::string::npos) continue;
        obj[part.substr(0, pos)] = part.substr(pos + 1);
    }
    return obj;
}

std::string encode_profile(const json& profile) {
    std::string buf;
    for (auto& [key, value] : profile.items()) {
        if (!buf.empty()) {
            buf += '&';
        }
        buf += key + '=' + value.get<std::string>();
    }
    return buf;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string tok;

    while (std::getline(ss, tok, delim)) {
        tokens.push_back(tok);  
    }

    return tokens;
}

std::vector<BYTE> encrypt_profile(const json& profile, const std::vector<BYTE>& key) {
    std::string plaintext = encode_profile(profile);

    const EVP_CIPHER* cipher = EVP_aes_128_ecb();
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, cipher, nullptr, key.data(), nullptr);
    EVP_CIPHER_CTX_set_padding(ctx, 1);

    std::vector<BYTE> ct(plaintext.size() + EVP_CIPHER_block_size(cipher));
    int len = 0, final_len = 0;

    EVP_EncryptUpdate(ctx, ct.data(), &len, (BYTE*)plaintext.data(), plaintext.size());
    EVP_EncryptFinal_ex(ctx, ct.data() + len, &final_len);

    EVP_CIPHER_CTX_free(ctx);
    ct.resize(len + final_len);
    return ct;
}

json decrypt_profile(const std::vector<BYTE>& ct, const std::vector<BYTE>& key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, key.data(), nullptr);
    EVP_CIPHER_CTX_set_padding(ctx, 1);

    std::vector<BYTE> pt(ct.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, final_len = 0;

    EVP_DecryptUpdate(ctx, pt.data(), &len, ct.data(), ct.size());
    EVP_DecryptFinal_ex(ctx, pt.data() + len, &final_len);
    pt.resize(len + final_len);

    EVP_CIPHER_CTX_free(ctx);

    return parse_kv(bytes_to_ascii(pt));
}


std::vector<BYTE> rand_bytes(const size_t len) {
    std::vector<BYTE> buffer(len);
    if (RAND_bytes(buffer.data(), buffer.size()) != 1) 
        throw std::runtime_error("RAND_bytes failed");
    return buffer;
}

