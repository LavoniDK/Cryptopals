#include <iostream>
#include <vector> 

#include "ascii.h"
#include "hex.h"

using BYTE = unsigned char;

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input string>\n";
        return 1;
    }

    std::string input_string(argv[1]);
    std::vector<BYTE> input = ascii_to_bytes(input_string);
    
    const int BLOCK_SIZE = 16;
    int pad = BLOCK_SIZE - (input.size() % BLOCK_SIZE);
    if (pad == 0) pad = BLOCK_SIZE;

    for (size_t i = 0; i < pad; i++) {
        input.push_back(static_cast<BYTE>(pad));  
    } 
    
    std::cout << "Padded bytes (ASCII): " << bytes_to_ascii(input) << '\n';

    std::cout << "Padded bytes (hex): " << bytes_to_hex(input) << '\n';

    return 0;

}
