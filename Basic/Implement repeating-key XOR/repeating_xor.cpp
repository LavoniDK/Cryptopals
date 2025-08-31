#include <iostream>
#include <string>
#include <vector>
#include "hex.h"
#include "ascii.h"

// --- Function Prototypes ---
std::vector<BYTE> repeating_xor(const std::string& pt, const std::string& key);

// --- Main Program ---
int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Error: you need to provide exactly two operands (plaintext and key)" << std::endl;
        return 1;
    }

    std::string plaintext = argv[1];
    std::string key = argv[2];

    std::vector<BYTE> ciphertext_bytes = repeating_xor(plaintext, key);
    std::string ciphertext_hex = bytes_to_hex(ciphertext_bytes);
    std::cout << ciphertext_hex << std::endl;
    return 0;
}

// --- Function Implementations ---

std::vector<BYTE> repeating_xor(const std::string& pt, const std::string& key) {
    
    std::vector<BYTE> pt_bytes = ascii_to_bytes(pt);
    std::vector<BYTE> key_bytes = ascii_to_bytes(key);
    
    size_t len = pt_bytes.size();
    std::vector<BYTE> result; 
    result.reserve(len);

    for (size_t i = 0; i < len; i++) {
        result.push_back(pt_bytes[i] ^ key_bytes[i % key_bytes.size()]);
    }
    return result;
}
