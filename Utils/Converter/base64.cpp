#include <vector>
#include <string>
#include <stdexcept>
#include <iterator>

#include "base64.h"

using BYTE = unsigned char;

// --- Function Prototypes ---
char int_to_base64_char(unsigned int x);
int base64_char_to_int(char c);

// --- Function Implementations ---
std::string bytes_to_base64(const std::vector<BYTE>& bytes, bool enable_padding) {
    
    std::string b64;
    b64.reserve(((bytes.size() / 3) + (bytes.size() % 3 > 0)) * 4);
    
    auto it = bytes.cbegin();
    while (std::distance(it, bytes.cend()) >= 3) {
        unsigned int b0 = *it++;
        unsigned int b1 = *it++;
        unsigned int b2 = *it++;
        unsigned int triple = (b0 << 16) | (b1 << 8) | b2;
        b64.push_back(int_to_base64_char((triple >> 18) & 0x3F));
        b64.push_back(int_to_base64_char((triple >> 12) & 0x3F));
        b64.push_back(int_to_base64_char((triple >> 6) & 0x3F));
        b64.push_back(int_to_base64_char(triple & 0x3F));
    }

    int remaining_bytes = std::distance(it, bytes.cend());
    if (remaining_bytes == 1) {
        unsigned int b0 = *it++;
        unsigned int triple = b0 << 16;
        b64.push_back(int_to_base64_char((triple >> 18) & 0x3F));
        b64.push_back(int_to_base64_char((triple >> 12) & 0x3F));
        if (enable_padding) {
            b64 += "==";
        }
    } else if (remaining_bytes == 2) {
        unsigned int b0 = *it++;
        unsigned int b1 = *it++;
        unsigned int triple = (b0 << 16) | (b1 << 8);
        b64.push_back(int_to_base64_char((triple >> 18) & 0x3F));
        b64.push_back(int_to_base64_char((triple >> 12) & 0x3F));
        b64.push_back(int_to_base64_char((triple >> 6) & 0x3F));
        if (enable_padding) {
            b64 += "=";
        }
    }

    return b64;
}

char int_to_base64_char(unsigned int x) {
    static const char b64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    if (x >= 64) {
        throw std::invalid_argument("Input to int_to_base64_char must be less than 64.");
    }
    return b64_chars[x];
}

std::vector<BYTE> base64_to_bytes(const std::string& b64) {

    if (b64.length() % 4 != 0) 
        throw std::invalid_argument("Base64 string is misaligned");

    std::vector<BYTE> bytes;
    bytes.reserve((b64.length() / 4) * 3);

    auto it = b64.cbegin();
    while (std::distance(it, b64.cend()) >= 4) {
        unsigned int c0 = base64_char_to_int(*it++);
        unsigned int c1 = base64_char_to_int(*it++);
        unsigned int c2 = base64_char_to_int(*it++);
        unsigned int c3 = base64_char_to_int(*it++);
        unsigned int quad = (c0 << 18) | (c1 << 12) | (c2 << 6) | c3;

        bytes.push_back((quad >> 16) & 0xFF);

        if (*std::prev(it, 2) != '=')
            bytes.push_back((quad >> 8) & 0xFF);
        if (*std::prev(it, 1) != '=')
            bytes.push_back(quad & 0xFF);
    }

    return bytes;
}

int base64_char_to_int(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';          
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;      
    if (c >= '0' && c <= '9') return c - '0' + 52;       
    if (c == '+')             return 62;                  
    if (c == '/')             return 63;                    
    if (c == '=')             return 0;                    
    throw std::invalid_argument("Invalid Base64 character");
}