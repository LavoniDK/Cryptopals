#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iterator> 

std::string hex_to_base64(const std::string& hex, bool enable_padding);
int hex_char_to_int(char c);
char int_to_base64_char(unsigned int x);

int main(int argc, char* argv[]) {
    // --- ARGUMENT PARSING LOGIC ---
    
    bool padding_enabled = false;
    std::string hex_input;
    
    bool parity_strict = true;    

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--pad") {
            padding_enabled = true;
        } else if (arg == "--nonstrict") {
            parity_strict = false;   
        } else {
            hex_input = arg;
        }
    }

    // --- INPUT VALIDATION ---
    
    if (hex_input.empty()) {
        std::cerr << "Error: No hex string provided." << std::endl;
        std::cerr << "Usage: " << argv[0] << " [--pad] [--nonstrict] <hex_string>" << std::endl;
        return 1; 
    }

    if (hex_input.length() % 2 != 0 && parity_strict) {
        throw std::invalid_argument("Hex string must have an even number of characters.");
    }

    // --- EXECUTION ---
    
    try {
        std::cout << hex_to_base64(hex_input, padding_enabled) << '\n';
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

std::string hex_to_base64(const std::string& hex, bool enable_padding) {
    std::vector<unsigned char> bytes;
    bytes.reserve(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        int high = hex_char_to_int(hex[i]);
        int low = hex_char_to_int(hex[i+1]);
        bytes.push_back((high << 4) | low);
    }

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

int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::invalid_argument("Invalid hexadecimal character.");
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
