#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iterator> 

#include "hex.h"
#include "base64.h" 

using BYTE = unsigned char;

int main(int argc, char* argv[]) {
    // --- ARGUMENT PARSING ---
    
    std::string hex_input;
    bool padding_enabled = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--no-pad") {
            padding_enabled = false;
        } else {
            hex_input = arg;
        }
    }

    // --- INPUT VALIDATION ---
    
    if (hex_input.empty()) {
        std::cerr << "Error: No hex string provided." << std::endl;
        std::cerr << "Usage: " << argv[0] << " [--no-pad] <hex_string>" << std::endl;
        return 1; 
    }

    // --- EXECUTION ---
    
    try {
        // Step 1: Convert the input hex string into our common byte format
        std::vector<BYTE> bytes = hex_to_bytes(hex_input);

        // Step 2: Convert the bytes into the final Base64 string
        std::string base64_output = bytes_to_base64(bytes, padding_enabled);
    
        // Step 3: Print the result
        std::cout << base64_output << std::endl;

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

