#include <cmath>     
#include <iostream>  
#include <cctype>    

#include "xor.h" 

using BYTE = unsigned char;

// --- Function Implementations ---

std::vector<BYTE> fixed_xor(const std::vector<BYTE>& bytes1, const std::vector<BYTE>& bytes2) {

    if (bytes1.size() != bytes2.size()) {
        throw std::invalid_argument("Input buffers for XOR must have the same length.");
    }   
    std::vector<BYTE> product;
    product.reserve(bytes1.size());
 
    for (size_t i = 0; i < bytes1.size(); i++) {
        product.push_back(bytes1[i] ^ bytes2[i]);
    }
    return product;
}

std::vector<BYTE> constant_xor(const std::vector<BYTE>& bytes, const BYTE key) {

    std::vector<BYTE> product;
    product.reserve(bytes.size());
 
    for (size_t i = 0; i < bytes.size(); i++) {
        product.push_back(bytes[i] ^ key);
    }

    return product;
}

void void_constant_xor(std::vector<BYTE>& bytes, const BYTE key) {

    for (size_t i = 0; i < bytes.size(); i++) {
        bytes[0] = bytes[i] ^ key;
    }
}

std::vector<BYTE> repeating_xor(const std::vector<BYTE>& bytes, const std::vector<BYTE>& key) {
    
    size_t len = bytes.size();
    std::vector<BYTE> result; 
    result.reserve(len);

    for (size_t i = 0; i < len; i++) {
        result.push_back(bytes[i] ^ key[i % key.size()]);
    }
    return result;
}

void void_repeating_xor(std::vector<BYTE>& bytes, const std::vector<BYTE>& key) {

    for (size_t i = 0; i < bytes.size(); i++) {
        bytes[i] = (bytes[i] ^ key[i % key.size()]);
    }
}

BYTE get_single_byte_xor_key(const std::vector<BYTE>& ct_bytes) {
         
    double best_score = 1e9;
    std::string best_pt = "";
    BYTE best_key = 0;

    for (int key = 0; key <= 255; ++key) {
        std::vector<BYTE> pt_bytes = constant_xor(ct_bytes, static_cast<BYTE>(key));
        std::string pt_str(pt_bytes.begin(), pt_bytes.end());

        double score = chi_squared(pt_bytes);
        
        if (score < best_score) {
            best_pt = pt_str;
            best_score = score;
            best_key = static_cast<BYTE>(key);
        }
    }

    return best_key;
}

double chi_squared(const std::vector<BYTE>& bytes) {
    
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

    std::map<char, int> observations;
    int letter_count = 0;
    double penalty = 0.0;
    
    for (char c : bytes) {
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