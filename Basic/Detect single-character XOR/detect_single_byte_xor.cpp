#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <tuple>
#include <fstream>
#include <sstream>

#include "hex.h"
#include "xor.h"

using BYTE = unsigned char;

// --- Function Prototypes ---
std::tuple<std::string, double> get_score_and_text(const std::vector<BYTE>& bytes);

// --- Entry Point ---
int main() {
    
    std::ifstream input_file("4.txt");
    
    std::vector<std::vector<BYTE>> bytes_lines;
    std::string line;
    while (std::getline(input_file, line)) {
        std::istringstream iss(line);
        std::string hex_line;
        if (iss >> hex_line) {
                bytes_lines.push_back(hex_to_bytes(hex_line));
        }
    }
    input_file.close();
    std::multimap<double, std::string> ranked_texts;
    for (std::vector<BYTE> line : bytes_lines) {
        auto tup = get_score_and_text(line);
        std::string text;
        double score;
        std::tie(text, score) = tup;
        ranked_texts.insert({score, text});
    }
    
    std::cout << "Top 10 pt was:" << '\n';
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
    std::string best_pt = "";

    for (int key = 0; key <= 255; ++key) {
        std::vector<BYTE> pt_bytes = constant_xor(bytes, static_cast<BYTE>(key));
        std::string pt_str(pt_bytes.begin(), pt_bytes.end());

        double score = chi_squared(pt_bytes);
        
        if (score < best_score) {
            best_pt = pt_str;
            best_score = score;
        }
    }
    return std::make_tuple(best_pt, best_score);
}