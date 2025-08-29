#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream> 
#include <iomanip> 

// --- Function Prototypes ---
unsigned int hex_char_to_int(unsigned char c);
std::vector<unsigned char> hex_to_bytes(const std::string& hex);
std::string bytes_to_hex(const std::vector<unsigned char>& bytes); 
std::vector<unsigned char> xor_of_bytes(const std::vector<unsigned char>& buf1, const std::vector<unsigned char>& buf2);


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
        
        std::vector<unsigned char> product = xor_of_bytes(bytes1, bytes2);
        
        std::cout << "Result: " << bytes_to_hex(product) << '\n';   
   
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; 
    }
    return 0;
}


// --- Function Implementations ---

std::vector<unsigned char> xor_of_bytes(const std::vector<unsigned char>& buf1, const std::vector<unsigned char>& buf2) {
    if (buf1.size() != buf2.size()) {
        throw std::invalid_argument("Input buffers for XOR must have the same length.");
    }   

    std::vector<unsigned char> product;
    product.reserve(buf1.size());
 
    for (size_t i = 0; i < buf1.size(); i++) {
        product.push_back(buf1[i] ^ buf2[i]);
    }

    return product;
}

std::vector<unsigned char> hex_to_bytes(const std::string& hex) {
    if (hex.length() % 2 != 0) {
        throw std::invalid_argument("Hex string must have an even number of characters.");
    }

    std::vector<unsigned char> bytes;
    bytes.reserve(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        int high = hex_char_to_int(hex[i]);
        int low = hex_char_to_int(hex[i + 1]);
        bytes.push_back((high << 4) | low);
    }
    return bytes;
}

unsigned int hex_char_to_int(unsigned char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    throw std::invalid_argument("Invalid hexadecimal character.");
}

std::string bytes_to_hex(const std::vector<unsigned char>& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned char byte : bytes) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}
