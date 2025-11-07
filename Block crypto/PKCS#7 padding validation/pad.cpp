#include <iostream>
#include <vector>
#include <string>
#include <exception>

#include "ascii.h"

using BYTE = unsigned char;
using bv = std::vector<BYTE>;

// --- PROTOTYPES --- 
bv pkcs_unpad(const bv& pt, size_t block = 16);

// --- GLOBAL CONSTANTS ---
const std::string valid = "ICE ICE BABY\x04\x04\x04\x04";
const std::string invalid1 = "ICE ICE BABY\x05\x05\x05\x05";
const std::string invalid2 = "ICE ICE BABY\x01\x02\x03\x04";

int main() {
    
    try {
        std::cout << bytes_to_ascii(pkcs_unpad(ascii_to_bytes(valid))) << std::endl;
    } catch (...) {
        std::cout << "Plaintext " << valid << " has invalid padding" << std::endl;
    }

    try {
        std::cout << bytes_to_ascii(pkcs_unpad(ascii_to_bytes(invalid1))) << std::endl;
    } catch (...) {
        std::cout << "Plaintext " << invalid1 << " has invalid padding" << std::endl;
    }

    try {
        std::cout << bytes_to_ascii(pkcs_unpad(ascii_to_bytes(invalid2))) << std::endl;
    } catch (...) {
        std::cout << "Plaintext " << invalid2 << " has invalid padding" << std::endl;
    }

    return 0;
}

bv pkcs_unpad(const bv& pt, size_t block_size) {

    if (pt.size() == 0 || pt.size() % block_size != 0)
        throw std::runtime_error("Invalid padding");

    BYTE pad_len = pt.back();

    if (pad_len == 0 || pad_len > block_size)
        throw std::runtime_error("Invalid padding");

    size_t pad_start = pt.size() - pad_len;

    for (size_t i = pad_start; i < pt.size(); ++i) {
        if (pt[i] != pad_len)
            throw std::runtime_error("Invalid padding");
    }

    return bv(pt.begin(), pt.end() - pad_len);
}
