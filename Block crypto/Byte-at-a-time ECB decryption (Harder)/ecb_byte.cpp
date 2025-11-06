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
#include "linalg.h"

using BYTE = unsigned char;
using bv = std::vector<BYTE>;
using bvv = std::vector<std::vector<BYTE>>;

// --- PROTOTYPES --- 
bool is_ecb(const bv& ct, size_t block_size);
bv rand_bytes(const size_t len);
bv oracle(const bv& infix);
size_t detect_block_size();
bv decryption_oracle(size_t prefix_len, size_t block_size);
size_t learn_prefix_len(size_t block_size);

// --- GLOBAL CONSTANTS ---
std::string secret_b64 = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";
bv secret_bytes = base64_to_bytes(secret_b64);
const size_t real_prefix_len = 64;
const bv prefix = rand_bytes(real_prefix_len);
const bv key = rand_bytes(16);

int main() {

    // 1.  Learn block size
    size_t block_size = detect_block_size();

    // 2. Learn MoO
    bv ecb_check_pt(block_size * 3, 'A');
    bv ecb_check_ct = oracle(ecb_check_pt);
    bool ecb = is_ecb(ecb_check_ct, block_size);
    
    // Sanity Check
    assert(ecb);
    std::cout << "Detected AES-ECB-" << block_size * 8 << std::endl;

    // learn prefix delimiter position
    size_t prefix_len = learn_prefix_len(block_size);
    assert(prefix_len == real_prefix_len);
    
    bv pt = decryption_oracle(real_prefix_len, block_size);
    
    std::cout << bytes_to_ascii(pt) << std::endl;

    return 0;
}

bv decryption_oracle(size_t prefix_len, size_t block_size) {
    
    bv learned;
    size_t pre_pad = (block_size - (prefix_len % block_size)) % block_size;
    size_t base_block_idx = (prefix_len + pre_pad) / block_size;

    while(true) {

        size_t offset = learned.size() % block_size;
        size_t pad_len = (block_size - 1) - offset;
        bv pad(pad_len + pre_pad, 'A');
        bv ct_pad = oracle(pad);
        
        size_t tgt_block = base_block_idx + (learned.size() / block_size);
        if ((tgt_block + 1) * block_size > ct_pad.size()) break;
        bv target_block(
            ct_pad.begin() + tgt_block * block_size,
            ct_pad.begin() + (tgt_block + 1) * block_size
        );

        bool found = false;
        for (int b = 0; b < 256; ++b) {
            BYTE guess = static_cast<BYTE>(b);

            // make the payload: prepad + prefix + learned + guess
            bv pl = pad;
            pl.insert(pl.end(), learned.begin(), learned.end());
            pl.push_back(guess);

            // encrypt under oracle
            bv pl_ct = oracle(pl);
            bv pl_blk(pl_ct.begin() + tgt_block * block_size,
                      pl_ct.begin() + (tgt_block + 1) * block_size);

            // check for match
            if (pl_blk == target_block) {
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
    return learned;
}

size_t learn_prefix_len(size_t block_size) {
   
    bv base_a(16, 'A'); 
    bv base_b(1, 'B'); 
    bv ct_a = oracle(base_a); 
    bv ct_b = oracle(base_b); 
   
    size_t block_idx; 
   
    for (block_idx = 0; block_idx < ct_a.size(); ++block_idx) { 
        if (split_vector(ct_a, block_size)[block_idx] != split_vector(ct_b, block_size)[block_idx]) { 
            break; 
        } 
    } 

    size_t byte_idx; 
    for (byte_idx = 1; byte_idx <= block_size; ++byte_idx) { 
        base_b.insert(base_b.begin(), 'A'); 
        bv a = split_vector(ct_a, block_size)[block_idx];
        bv b = split_vector(oracle(base_b), block_size)[block_idx];
        if (a == b) { 
            break; 
        } 
    }
    
    return (block_idx * block_size) + (block_size - byte_idx);
} 

size_t detect_block_size() {
    bv empty;
    size_t base_len = oracle(empty).size();

    for (size_t i = 1; i <= 256; ++i) {
        std::string s(i, 'A');                   
        bv prefix(s.begin(), s.end());
        size_t new_len = oracle(prefix).size();
        if (new_len > base_len) {
            return new_len - base_len; 
        }
    }
    return 0; // error
}

bv oracle(const bv& infix) {
    
    bv pt = prefix;
    pt.insert(pt.end(), infix.begin(), infix.end());
    pt.insert(pt.end(), secret_bytes.begin(), secret_bytes.end());
    const EVP_CIPHER* cipher = EVP_aes_128_ecb();
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, cipher, nullptr, key.data(), nullptr);
    bv ct(pt.size() + EVP_CIPHER_block_size(cipher));
    int upd_len = 0;
    int fin_len = 0;
    
    EVP_EncryptUpdate(ctx, ct.data(), &upd_len, pt.data(), (int) pt.size());
    EVP_EncryptFinal_ex(ctx, ct.data() + upd_len, &fin_len);
    
    EVP_CIPHER_CTX_free(ctx);
    ct.resize(upd_len + fin_len);

    return ct; 
}

bool is_ecb(const bv& ct, size_t block_size) {
    std::set<bv> seen;
    for (size_t i = 0; i + block_size <= ct.size(); i += block_size) {
        bv block(ct.begin() + i, ct.begin() + i + block_size);
        if (!seen.insert(block).second)
            return true; // duplicate block detected 
    }
    return false;
}

bv rand_bytes(const size_t len) {
    bv buffer(len);
    if (RAND_bytes(buffer.data(), (int) buffer.size()) != 1) 
        throw std::runtime_error("RAND_bytes failed");
    return buffer;
}
