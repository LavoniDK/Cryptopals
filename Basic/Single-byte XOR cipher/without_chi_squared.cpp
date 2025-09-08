#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>

#include "hex.h"
#include "xor.h"

using BYTE = unsigned char;

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


int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        std::cerr << "Error: you need to provide exactly one operand" << std::endl;
        return 1; 
    }
     
    std::string ciphertext_hex = argv[1];  
    std::vector<BYTE> ciphertext_bytes = hex_to_bytes(ciphertext_hex);
    
    double best_score = -1.0;
    std::string best_plaintext = "";
    BYTE best_key = 0;

    for (int key = 0; key <= 255; ++key) {
        std::vector<BYTE> plaintext_bytes = constant_xor(ciphertext_bytes, static_cast<BYTE>(key));
        std::string plaintext_str(plaintext_bytes.begin(), plaintext_bytes.end());

        double score = get_plaintext_score(plaintext_str);
        
        if (score > best_score) {
            best_plaintext = plaintext_str;
            best_score = score;
            best_key = static_cast<BYTE>(key);
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