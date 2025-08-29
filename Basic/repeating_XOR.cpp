#include <iostream>
#include <string>
#include <sstream> 
#include <iomanip> 

// --- Function Prototypes ---
std::string bytes_to_hex(const std::string& bytes); 
std::string repeating_xor(const std::string& pt, const std::string& key);


// --- Main Program ---
int main() {

    std::string plaintext = "Burning 'em, if you ain't quick and nimble I go crazy when I hear a cymbal";
    std::string key = "ICE";
    
    std::string ciphertext = repeating_xor(plaintext, key);
    std::string ciphertext_hex = bytes_to_hex(ciphertext);
    std::cout << ciphertext_hex << "\n";    
    return 0;
}

// --- Function Implementations ---

std::string repeating_xor(const std::string& pt, const std::string& key) {
    
    std::string result;
    size_t len = pt.size();
    result.reserve(len);
    
    for (size_t i = 0; i < len; i++) {
        result.push_back(pt[i] ^ key[i % key.length()]);
    }
    return result;
}


std::string bytes_to_hex(const std::string& bytes) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned char byte : bytes) {
        ss << std::setw(2) << static_cast<int>(byte & 0xFF);
    }
    return ss.str();
}

