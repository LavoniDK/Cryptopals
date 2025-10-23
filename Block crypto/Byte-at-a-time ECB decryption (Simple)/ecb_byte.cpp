#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#include <cassert>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "ascii.h"
#include "base64.h"

using BYTE = unsigned char;

// --- PROTOTYPES --- 
bool is_ecb(const std::vector<BYTE>& ct, size_t block_size);
std::vector<BYTE> rand_bytes(const size_t len);
std::vector<BYTE> oracle(const std::vector<BYTE>& prefix);
size_t detect_block_size();

// --- GLOBAL CONSTANTS ---
std::string secret_b64 = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";
std::vector<BYTE> secret_bytes = base64_to_bytes(secret_b64);
const std::vector<BYTE> key = rand_bytes(16);

int main() {

    // 1.  Learn block size
    size_t block_size = detect_block_size();

    // 2. Learn MoO
    std::vector<BYTE> ecb_check_pt(block_size * 3, 'A');
    std::vector<BYTE> ecb_check_ct = oracle(ecb_check_pt);
    bool ecb = is_ecb(ecb_check_ct, block_size);
    
    // Sanity Check
    assert(ecb);
    assert(block_size == 16);
    std::cout << "Detected AES-ECB-128" << std::endl;

    // 3. Learn the first byte
    std::vector<BYTE> learned;
    size_t init_len = oracle({}).size();
    learned.reserve(init_len);

    for (size_t i = 0; i < init_len; ++i) {
        size_t prefix_len = (block_size - 1) - (i % block_size);
        std::vector<BYTE> prefix(prefix_len, 'A');
        std::vector<BYTE> ct = oracle(prefix);
        size_t block_idx = i / block_size;
        std::vector<BYTE> target_block(ct.begin() + (block_idx * block_size), ct.begin() + ((block_idx + 1) * block_size));

        bool found = false;
        for (int b = 0; b < 256; ++b) {
            BYTE guess = static_cast<BYTE>(b);
            
            // make the payload: prefix + learned + guess
            std::vector<BYTE> pl = prefix;
            pl.insert(pl.end(), learned.begin(), learned.end());
            pl.push_back(guess);

            // encrypt under oracle
            std::vector<BYTE> pl_ct = oracle(pl);
            std::vector<BYTE> pl_ct_block(pl_ct.begin() + (block_idx * block_size), pl_ct.begin() + ((block_idx + 1) * block_size));

            // check for match
            if (pl_ct_block == target_block) {
                learned.push_back(guess);
                found = true;
                break;
            }
        }

        if (!found) {
            // If this is reached we made a full iteration while learning a byte, meaning we are done
            break;
        }
    }

    std::cout << "Plaintext: \n" << bytes_to_ascii(learned) << std::endl;

    return 0;
}

size_t detect_block_size() {
    std::vector<BYTE> empty;
    size_t base_len = oracle(empty).size();

    for (size_t i = 1; i <= 256; ++i) {
        std::string s(i, 'A');                   
        std::vector<BYTE> prefix(s.begin(), s.end());
        size_t new_len = oracle(prefix).size();
        if (new_len > base_len) {
            return new_len - base_len; 
}
    }
    return 0; // error
}

std::vector<BYTE> oracle(const std::vector<BYTE>& prefix) {
    
    std::vector<BYTE> pt = prefix; 
    pt.insert(pt.end(), secret_bytes.begin(), secret_bytes.end());
    const EVP_CIPHER* cipher = EVP_aes_128_ecb();
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, cipher, nullptr, key.data(), nullptr);
    std::vector<BYTE> ct(pt.size() + EVP_CIPHER_block_size(cipher));
    int upd_len = 0;
    int fin_len = 0;
    
    EVP_EncryptUpdate(ctx, ct.data(), &upd_len, pt.data(), pt.size());
    EVP_EncryptFinal_ex(ctx, ct.data() + upd_len, &fin_len);
    
    EVP_CIPHER_CTX_free(ctx);
    ct.resize(upd_len + fin_len);

    return ct; 
}

bool is_ecb(const std::vector<BYTE>& ct, size_t block_size) {
    std::set<std::vector<BYTE>> seen;
    for (size_t i = 0; i + block_size <= ct.size(); i += block_size) {
        std::vector<BYTE> block(ct.begin() + i, ct.begin() + i + block_size);
        if (!seen.insert(block).second)
            return true; // duplicate block detected 
    }
    return false;
}

std::vector<BYTE> rand_bytes(const size_t len) {
    std::vector<BYTE> buffer(len);
    if (RAND_bytes(buffer.data(), buffer.size()) != 1) 
        throw std::runtime_error("RAND_bytes failed");
    return buffer;
}
