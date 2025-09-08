#include <stdexcept>
#include <vector>
#include <string>

#include "hex.h"

using BYTE = unsigned char;

// --- Function Prototypes ---
inline char int_to_hex(BYTE i);
int hex_char_to_int(char c);

/// --- Function Implementations ---
std::string bytes_to_hex(const std::vector<BYTE>& bytes) {
    
    std::string hex;
    hex.reserve(bytes.size() * 2);

    for (BYTE b : bytes) {
        BYTE higher = (b >> 4) & 0x0F;
        BYTE lower = b & 0x0F;

        hex.push_back(int_to_hex(higher));
        hex.push_back(int_to_hex(lower));
    }

    return hex;
}

inline char int_to_hex(BYTE i) {
    const char hex_chars[] = "0123456789abcdef";
    return hex_chars[i];
}

std::vector<BYTE> hex_to_bytes(const std::string& hex) {
    
    if (hex.length() % 2 != 0) {
        throw std::invalid_argument("Hex string must have an even number of characters.");
    }

    std::vector<BYTE> bytes;
    bytes.reserve(hex.length() / 2);

    for (size_t i = 0; i < hex.length(); i += 2) {
        int high = hex_char_to_int(hex[i]);
        int low = hex_char_to_int(hex[i+1]);
        bytes.push_back((high << 4) | low);
    }
    
    return bytes;
}

int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::invalid_argument("Invalid hexadecimal character.");
}

