#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "hex.h"
#include "base64.h"
#include "ascii.h"

using BYTE = unsigned char

void print_usage() {
    std::cerr << "Usage: ./converter <input_format> <output_format> <data>" << std::endl;
    std::cerr << "Formats: hex, b64, ascii" << std::endl;
    std::cerr << "Example: ./converter hex b64 48656c6c6f" << std::endl;
}

std::vector<BYTE> to_bytes(const std::string& format, const std::string& data) {
    if (format == "hex") {
        return hex_to_bytes(data);
    } else if (format == "b64") {
        return base64_to_bytes(data);
    } else if (format == "ascii") {
        return ascii_to_bytes(data);
    }
    throw std::invalid_argument("Unknown input format: " + format);
}

std::string from_bytes(const std::string& format, const std::vector<BYTE>& bytes) {
    if (format == "hex") {
        return bytes_to_hex(bytes);
    } else if (format == "b64") {
        return bytes_to_base64(bytes);
    } else if (format == "ascii") {
        return bytes_to_ascii(bytes);
    }
    throw std::invalid_argument("Unknown output format: " + format);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        print_usage();
        return 1;
    }

    try {
        std::string input_format = argv[1];
        std::string output_format = argv[2];
        std::string data = argv[3];

        // Step 1: Convert to interface (bytes)
        std::vector<BYTE> intermediate_bytes = to_bytes(input_format, data);

        // Step 2: Convert bytes to specified output format
        std::string result = from_bytes(output_format, intermediate_bytes);

        std::cout << result << std::endl;

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        print_usage();
        return 1;
    }

    return 0; 
}
