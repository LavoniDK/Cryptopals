#include <cstdlib>
#include <vector>
#include <string>
#include <stdexcept>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <ctime>
#include <tuple>
#include <iostream>
#include "ascii.h"
#include <set>

using BYTE = unsigned char;
int BS = 16;

std::vector<BYTE> rand_bytes(const size_t len);
std::tuple<std::vector<BYTE>, std::string> enc_oracle(std::vector<BYTE>& plaintext);
std::string detector(const std::vector<BYTE>& ct);

int main (int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <plaintext>\n";
        return 1;
    }
    std::vector<BYTE> pt = ascii_to_bytes(argv[1]);
    
    if (pt.size() < static_cast<size_t>(3 * BS)) {
        std::cout << "Cannot distinguish MoO if message is shorter than 3 blocks.";
        return 1;
    }

    std::tuple<std::vector<BYTE>, std::string> oracle = enc_oracle(pt);
    auto ct = std::get<0>(oracle);
    auto mode = std::get<1>(oracle);
    
    std::string detected = detector(ct);
    
    std::cout << "Real MoO is: " << mode << std::endl
              << "Detected MoO is:" << detected << std::endl;
    
    return 0;
}

std::string detector(const std::vector<BYTE>& ct) {
    std::set<std::vector<BYTE>> seen;
    for (size_t i = 0; i + BS <= ct.size(); i += BS) {
        std::vector<BYTE> block(ct.begin() + i, ct.begin() + i + BS);
        if (!seen.insert(block).second)
            return "ECB"; // duplicate block detected
    }
    return "CBC";
}

std::tuple<std::vector<BYTE>, std::string> enc_oracle(std::vector<BYTE>& plaintext){
    
    srand(time(NULL));
    auto rand_len = []() -> size_t {return 5 + (rand() % 6);};
    size_t prefix_len = rand_len();
    size_t suffix_len = rand_len();
    
    std::vector<BYTE> prefix = rand_bytes(prefix_len);
    std::vector<BYTE> suffix = rand_bytes(suffix_len);

    std::vector<BYTE> new_pt;
    new_pt.reserve(prefix.size() + plaintext.size() + suffix.size());
    new_pt.insert(new_pt.end(), prefix.begin(), prefix.end());
    new_pt.insert(new_pt.end(), plaintext.begin(), plaintext.end());
    new_pt.insert(new_pt.end(), suffix.begin(), suffix.end());
    plaintext = std::move(new_pt);
    
    std::vector<BYTE> key = rand_bytes(BS);
    std::vector<BYTE> iv = rand_bytes(BS);

    std::string mode;
    const EVP_CIPHER* cipher_type;

    if (rand() % 2 == 0) { // Then CBC
        mode = "CBC";
        cipher_type = EVP_aes_128_cbc();
    } else { // Then ECB
        mode = "ECB";
        cipher_type = EVP_aes_128_ecb();
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, cipher_type, nullptr, key.data(), mode == "CBC" ? iv.data() : nullptr);

    std::vector<BYTE> ciphertext(plaintext.size() + EVP_CIPHER_block_size(cipher_type));
    int upd_len = 0;
    int fin_len = 0;
    
    EVP_EncryptUpdate(ctx, ciphertext.data(), &upd_len, plaintext.data(), plaintext.size());

    EVP_EncryptFinal_ex(ctx, ciphertext.data() + upd_len, &fin_len);

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(upd_len + fin_len);

    return std::make_tuple(ciphertext, mode); 
}

std::vector<BYTE> rand_bytes(const size_t len) {
    std::vector<BYTE> buffer(len);
    if (RAND_bytes(buffer.data(), buffer.size()) != 1) 
        throw std::runtime_error("RAND_bytes failed");
    return buffer;
}







