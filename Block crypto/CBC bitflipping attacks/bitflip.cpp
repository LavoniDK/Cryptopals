#include <iostream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "ascii.h"

using byte = unsigned char;
using bv = std::vector<byte>;

// --- PROTOTYPES --- 
std::vector<BYTE> rand_bytes(const size_t len);
bv surround_and_encrypt(const std::string& infix);
bv encrypt(const bv& pt, const bv& iv, const bv& key);
bv decrypt(const bv& ct, const bv& iv, const bv& key);
bool is_admin(const bv& ct);
void test();

// --- GLOBAL CONSTANTS ---
size_t BLOCK_SIZE = 16;
std::string prepend_data = "comment1=cooking%20MCs;userdata=";
std::string append_data = ";comment2=%20like%20a%20pound%20of%20bacon";
bv key = rand_bytes(BLOCK_SIZE);
bv iv = rand_bytes(BLOCK_SIZE);

// --- ENTRY POINT ---
int main() {
    std::string pl = "2:admin<true";
    bv ct = surround_and_encrypt(pl);

    // find the 2 bits in block 2 that changes the ciphertext such that the ":" and <" in the block 2
    // is changed to "=" (by flipping the last bit of that byte)
    
    int delim1_idx = prepend_data.length() + 1 - BLOCK_SIZE; // remove <BLOCK_SIZE> bytes because of IV
    int delim2_idx = prepend_data.length() + 7 - BLOCK_SIZE;
  
    ct[delim1_idx] ^= 1u;
    ct[delim2_idx] ^= 1u;

    if (is_admin(ct)) {
        std::cout << "Flipped bit 8 " << "of byte " << delim1_idx + 1 
                  << " and " << delim2_idx + 1
                  << " in ciphertext block " << (delim1_idx / BLOCK_SIZE) << "\n";
    }    
}


// --- HELPER FUNCTIONS ---
bv surround_and_encrypt(const std::string& infix) {

    std::string sanitized;
    for (char c : infix) {
        if (c == ';') sanitized += "%3B";
        else if (c == '=') sanitized += "%3D";
        else sanitized += c;
    }
    
    std::cout << prepend_data + sanitized + append_data << std::endl;

    bv pt = ascii_to_bytes(prepend_data + sanitized + append_data);
    return encrypt(pt, iv, key);
}

bool is_admin(const bv& ct) {
    bv pt = decrypt(ct, iv, key);
    std::string pt_str = bytes_to_ascii(pt);
    std::cout << pt_str << std::endl;
    return pt_str.find(";admin=true;") != std::string::npos;
}

bv rand_bytes(const size_t len) {
    bv buffer(len);
    if (RAND_bytes(buffer.data(), buffer.size()) != 1) 
        throw std::runtime_error("RAND_bytes failed");
    return buffer;
}

bv encrypt(const bv& pt, const bv& iv, const bv& key) {
    
    const EVP_CIPHER* cipher = EVP_aes_128_cbc();
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, cipher, nullptr, key.data(), iv.data());
    bv ct(pt.size() + EVP_CIPHER_block_size(cipher));
    int upd_len = 0;
    int fin_len = 0;
    
    EVP_EncryptUpdate(ctx, ct.data(), &upd_len, pt.data(), (int) pt.size());
    EVP_EncryptFinal_ex(ctx, ct.data() + upd_len, &fin_len);
    
    EVP_CIPHER_CTX_free(ctx);
    ct.resize(upd_len + fin_len);

    return ct; 
}

bv decrypt(const bv& ct, const bv& iv, const bv& key) {
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key.data(), iv.data());
    EVP_CIPHER_CTX_set_padding(ctx, 1);

    bv pt(ct.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, final_len = 0;

    EVP_DecryptUpdate(ctx, pt.data(), &len, ct.data(), ct.size());
    EVP_DecryptFinal_ex(ctx, pt.data() + len, &final_len);
    pt.resize(len + final_len);

    EVP_CIPHER_CTX_free(ctx);
    return pt;
}

void test() {
    std::string test = "foo=bar;admin=true;";
    std::cout << "Sanitized input: " << (is_admin(surround_and_encrypt(test)) ? "admin detected\n" : "no admin\n");
    std::cout << "Unsanitized input: " << (is_admin(encrypt(ascii_to_bytes(test), iv, key)) ? "admin detected\n" : "no admin\n");
}
