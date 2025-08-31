# include <iostream>
# include <vector>
# include <string>
# include <cmath>
# include <map>
# include <tuple>
# include <fstream>
# include <sstream>
# include "hex.h"

using BYTE = unsigned char;

// --- Function Prototypes ---
std::vector<BYTE> constant_XOR(const std::vector<BYTE>& data, BYTE key);
double chi_squared(const std::string& pt);
std::tuple<std::string, double> get_score_and_text(const std::vector<BYTE>& bytes);
    
// Global declaration
std::map<char, double> letter_frequencies = {
    {'e', 0.1202}, {'t', 0.0910}, {'a', 0.0812}, {'o', 0.0768},
    {'i', 0.0731}, {'n', 0.0695}, {'s', 0.0628}, {'r', 0.0602},
    {'h', 0.0592}, {'d', 0.0432}, {'l', 0.0398}, {'u', 0.0288},
    {'c', 0.0271}, {'m', 0.0261}, {'f', 0.0230}, {'y', 0.0211},
    {'w', 0.0209}, {'g', 0.0203}, {'p', 0.0182}, {'b', 0.0149},
    {'v', 0.0111}, {'k', 0.0069}, {'x', 0.0017}, {'q', 0.0011},
    {'j', 0.0010}, {'z', 0.0007}, {' ', 0.13000},{'.', 0.01000},
    {',', 0.0150}, {'-', 0.0040}, {'\'', 0.0050}
};

int main() {
    
    std::ifstream inputFile("4.txt");
    
    std::vector<std::vector<BYTE>> bytes_lines;
    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string hex_line;
        if (iss >> hex_line) {
                bytes_lines.push_back(hex_to_bytes(hex_line));
        }
    }
    inputFile.close();
    std::multimap<double, std::string> ranked_texts;
    for (std::vector<BYTE> line : bytes_lines) {
        auto tup = get_score_and_text(line);
        std::string text;
        double score;
        std::tie(text, score) = tup;
        ranked_texts.insert({score, text});
    }
    
    std::cout << "Top 10 Plaintext was:" << '\n';
    int count = 0;
    for (auto it = ranked_texts.begin(); it != ranked_texts.end() && count < 10; ++it) {
         std::cout << count + 1 << ": " << it->second << '\n';
        count++;
    }

    return 0;
}


// --- Function Implementations ---
std::tuple<std::string, double> get_score_and_text(const std::vector<BYTE>& bytes) {
    double best_score = 1e9;
    std::string best_plaintext = "";

    for (int key = 0; key <= 255; ++key) {
        std::vector<BYTE> plaintext_bytes = constant_XOR(bytes, static_cast<BYTE>(key));
        std::string plaintext_str(plaintext_bytes.begin(), plaintext_bytes.end());

        double score = chi_squared(plaintext_str);
        
        if (score < best_score) {
            best_plaintext = plaintext_str;
            best_score = score;
        }
    }
    return std::make_tuple(best_plaintext, best_score);
}

double chi_squared(const std::string& pt) {
    
    std::map<char, int> observations;
    int letter_count = 0;
    double penalty = 0.0;
    
    for (char c : pt) {
        if (!isprint(static_cast<unsigned char>(c)) && c != '\n' && c != '\t') {
            return 1e9;
        }
        char lower_c = std::tolower(static_cast<BYTE>(c));
        if (letter_frequencies.count(lower_c)) {
            observations[lower_c]++;
            letter_count++;
        } else {
            penalty += 25.0;
        }
    }

    if (letter_count == 0) {return 1e9;}
    
    double chi_squared_score = 0.0;
    for (const auto& pair : letter_frequencies) {
        char letter = pair.first;
        double expected_freq = pair.second;
        double expected_count = letter_count * expected_freq;
        
        int observed_count = 0;
        if (observations.count(letter)) {
            observed_count = observations.at(letter);
        }

        if (expected_count > 0) {
            double difference = observed_count - expected_count;
            chi_squared_score += std::pow(difference, 2) / expected_count;
        }
    }

    return chi_squared_score + penalty;
} 


std::vector<BYTE> constant_XOR(const std::vector<BYTE>& data, const BYTE key) {

    std::vector<BYTE> product;
    product.reserve(data.size());
 
    for (size_t i = 0; i < data.size(); i++) {
        product.push_back(data[i] ^ key);
    }

    return product;
}


