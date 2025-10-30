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

using BYTE = unsigned char;
using json = nlohmann::json;

json profile_for(const std::string& s);
std::string encode_profile(const json& profile);
std::vector<std::string> split(const std::string& s, char delim);
std::vector<BYTE> encrypt_profile(const json& profile, const std::vector<BYTE>& key, const std::vector<BYTE>& iv);
json decrypt_profile(const std::vector<BYTE>& ct, const std::vector<BYTE>& key, const std::vector<BYTE>& iv);
std::vector<BYTE> rand_bytes(const size_t len);

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    std::string input = argv[1];

    std::vector<BYTE> key = rand_bytes(16);
    std::vector<BYTE> iv = rand_bytes(16);

    json profile = profile_for(input);
    std::cout << profile.dump(4) << std::endl;
    std::cout << encode_profile(profile) << std::endl;
    std::cout << bytes_to_ascii(encrypt_profile(profile, key, iv)) << std::endl;
    std::cout << encode_profile(decrypt_profile(encrypt_profile(profile, key, iv), key, iv)) << std::endl;
    
    return 0;
}

json profile_for(const std::string& s) {
    std::vector<std::string> tokens = split(s, '&');
    json profile;
    for (std::string tok : tokens) {
        std::vector<std::string> kv = split(tok, '=');
        
        if (kv.size() > 2) { // Means we split around nested metachar, so we merge trailing substrings to 'eat' illegal chars
            for (size_t i = 2; i < kv.size(); ++i) {
                kv[1] += kv[i];
            }
        }
        profile[kv[0]] = kv[1];
    }
    return profile; 
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

std::vector<BYTE> encrypt_profile(const json& profile, const std::vector<BYTE>& key, const std::vector<BYTE>& iv) {
    std::string to_string = encode_profile(profile);
    
    const EVP_CIPHER* cipher_type = EVP_aes_128_cbc();
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, cipher_type, nullptr, key.data(), iv.data());

    std::vector<BYTE> ciphertext(to_string.size() + EVP_CIPHER_block_size(cipher_type));
    int upd_len = 0;
    int fin_len = 0;
    
    EVP_EncryptUpdate(ctx, ciphertext.data(), &upd_len, reinterpret_cast<const unsigned char*>(to_string.data()), to_string.size());

    EVP_EncryptFinal_ex(ctx, ciphertext.data() + upd_len, &fin_len);

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(upd_len + fin_len);

    return ciphertext; 
}

json decrypt_profile(const std::vector<BYTE>& ct, const std::vector<BYTE>& key, const std::vector<BYTE>& iv) { 
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    
    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key.data(), iv.data());
    
    std::vector<BYTE> pt(ct.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int pt_len = 0;

    EVP_DecryptUpdate(ctx, pt.data(), &len, ct.data(), ct.size());
    pt_len = len;

    EVP_DecryptFinal_ex(ctx, pt.data() + len, &len);    
    pt_len += len;

    EVP_CIPHER_CTX_free(ctx);

    pt.resize(pt_len);
    return profile_for(bytes_to_ascii(pt));
}


std::vector<BYTE> rand_bytes(const size_t len) {
    std::vector<BYTE> buffer(len);
    if (RAND_bytes(buffer.data(), buffer.size()) != 1) 
        throw std::runtime_error("RAND_bytes failed");
    return buffer;
}

