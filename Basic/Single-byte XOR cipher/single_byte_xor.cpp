#include <iostream>
#include <vector>
#include <stdexcept>

#include "hex.h"
#include "xor.h"
#include "ascii.h"

using BYTE = unsigned char;

// --- Function Prototypes ---
int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        std::cerr << "Error: you need to provide exactly one operand" << std::endl;
        return 1; 
    }
     
    std::vector<BYTE> ct_bytes = hex_to_bytes(argv[1]);
    BYTE key = get_single_byte_xor_key(ct_bytes);
    std::cout << bytes_to_ascii(constant_xor(ct_bytes, key)) << '\n';

    return 0;
}