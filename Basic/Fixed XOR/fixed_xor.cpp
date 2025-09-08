#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "hex.h"
#include "xor.h"

using BYTE = unsigned char;

// --- Main Program ---
int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Error: you need to provide exactly two operands" << std::endl;
        return 1; 
    }
       
    std::string hex_buf1 = argv[1];
    std::string hex_buf2 = argv[2];

    try {
        std::vector<unsigned char> bytes1 = hex_to_bytes(hex_buf1);
        std::vector<unsigned char> bytes2 = hex_to_bytes(hex_buf2);
        
        std::vector<unsigned char> product = fixed_xor(bytes1, bytes2);
        
        std::cout << "Result: " << bytes_to_hex(product) << '\n';   
   
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; 
    }
    return 0;
}
