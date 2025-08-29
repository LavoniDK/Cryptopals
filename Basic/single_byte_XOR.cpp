#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <cctype>

// --- Function Prototypes ---
std::vector<unsigned char> hex_to_bytes(const std::string& hex);
unsigned int hex_char_to_int(unsigned char c);
std::vector<unsigned char> constant_XOR(const std::vector<unsigned char>& data, unsigned char key);
double get_plaintext_score(const std::string& pt);


// Global declaration
std::map<char, double> letter_frequencies = {
    {'e', 0.1202}, {'t', 0.0910}, {'a', 0.0812}, {'o', 0.0768},
    {'i', 0.0731}, {'n', 0.0695}, {'s', 0.0628}, {'r', 0.0602},
    {'h', 0.0592}, {'d', 0.0432}, {'l', 0.0398}, {'u', 0.0288},
    {'c', 0.0271}, {'m', 0.0261}, {'f', 0.0230}, {'y', 0.0211},
    {'w', 0.0209}, {'g', 0.0203}, {'p', 0.0182}, {'b', 0.0149},
    {'v', 0.0111}, {'k', 0.0069}, {'x', 0.0017}, {'q', 0.0011},
    {'j', 0.0010}, {'z', 0.0007}, {' ', 0.0800}
};


int main() {
    
    std::string ciphertext_hex = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    std::vector<unsigned char> ciphertext_bytes = hex_to_bytes(ciphertext_hex);
    
    double best_score = -1.0;
    std::string best_plaintext = "";
    unsigned char best_key = 0;

    for (int key = 0; key <= 255; ++key) {
        std::vector<unsigned char> plaintext_bytes = constant_XOR(ciphertext_bytes, static_cast<unsigned char>(key));
        std::string plaintext_str(plaintext_bytes.begin(), plaintext_bytes.end());

        double score = get_plaintext_score(plaintext_str);
        
        if (score > best_score) {
            best_plaintext = plaintext_str;
            best_score = score;
            best_key = static_cast<unsigned char>(key);
        }
    }
    std::cout << "Plaintext found: " << best_plaintext << '\n'
              << "Score was: " << best_score << '\n'
              << "Key was: " << best_key << '\n';
            
    return 0;
}


// --- Function Implementations ---

double get_plaintext_score(const std::string& pt) {
    
    double total_score= 0.0;

    for (char c : pt) {
        char lower_c = std::tolower(c);
        if (letter_frequencies.count(lower_c)) {
             total_score += letter_frequencies.at(lower_c);
        }
    }
    return total_score; 
} 


std::vector<unsigned char> constant_XOR(const std::vector<unsigned char>& data, const unsigned char key) {

    std::vector<unsigned char> product;
    product.reserve(data.size());
 
    for (size_t i = 0; i < data.size(); i++) {
        product.push_back(data[i] ^ key);
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
