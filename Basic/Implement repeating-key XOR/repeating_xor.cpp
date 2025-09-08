#include <iostream>
#include <string>
#include <vector>

#include "hex.h"
#include "ascii.h"
#include "xor.h"

using BYTE = unsigned char;

// --- Main Program ---
int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Error: you need to provide exactly two operands (plaintext and key)" << std::endl;
        return 1;
    }

    std::string pt_str = argv[1];
    std::string key = argv[2];

    std::vector<BYTE> ct_bytes = repeating_xor(ascii_to_bytes(pt_str), ascii_to_bytes(key));
    std::string ct_hex = bytes_to_hex(ct_bytes);
    std::cout << ct_hex << std::endl;
    return 0;
}