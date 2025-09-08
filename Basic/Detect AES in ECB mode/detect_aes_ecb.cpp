#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

#include "hex.h"
#include "ascii.h"
#include "linalg.h"

using BYTE = unsigned char;

bool find_matching_block(const std::vector<std::vector<BYTE>>& blocks);

int main() {
    std::ifstream file("8.txt");
    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;

        std::vector<BYTE> byte_line = hex_to_bytes(line);

        std::vector<std::vector<BYTE>> splitted_line = split_vector(byte_line, 16);

        if (find_matching_block(splitted_line)) {
            std::cout << "ECB detected in line number: " << line_number << std::endl;
            std::cout << "Ciphertext: " << line << std::endl;
            break;
        }
    }

    file.close();
    return 0;
}

bool find_matching_block(const std::vector<std::vector<BYTE>>& blocks) {
    std::set<std::vector<BYTE>> unique_blocks;
    for (const auto& block : blocks) {
        if (!unique_blocks.insert(block).second) {
            return true;
        }
    }
    return false;
}
