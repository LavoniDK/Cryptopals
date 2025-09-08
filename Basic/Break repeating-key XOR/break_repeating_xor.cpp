#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype> 
#include <tuple>

#include "base64.h"
#include "ascii.h"
#include "xor.h"
#include "linalg.h"

using BYTE = unsigned char; 

// --- Prototypes ---
int hamming_distance(const std::vector<BYTE>& b1, const std::vector<BYTE>& b2);

std::multimap<float, std::tuple<int, int>> find_best_keylen(const std::vector<BYTE>& ct);

// --- Entry point ---
int main() {
    // --- Data ---

    std::ifstream input_file("6.txt");
    std::string b64_data;
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    b64_data = buffer.str();
    b64_data.erase(std::remove_if(b64_data.begin(), b64_data.end(), [](unsigned char c){ return std::isspace(c); }), b64_data.end());

    input_file.close();
    std::vector<BYTE> ct_bytes = base64_to_bytes(b64_data);
 
    // --- Pipeline ---
    
    // Find the best key size - by minimizing average pairwise normalized hamming distance for variable keysize and count of blocks to compare on
    std::multimap<float, std::tuple<int, int>> keylen_scores = find_best_keylen(ct_bytes); 
    auto it = keylen_scores.begin();
    unsigned int keylen = std::get<0>(it->second);

    // break the ct down into blocks of of size <largest key_length candidates>
    std::vector<std::vector<BYTE>> ct_blocks = split_vector(ct_bytes, keylen);

    // Transpose the blocks
    std::vector<BYTE> ct_bytes_copy = ct_bytes; // Copy due to side-effecting procedure
    void_transpose(ct_blocks);

    // Solve the repeating-key xor for each block, the solution for each block is the char in the corresponding position in the repeating key
    std::vector<BYTE> key;
    key.reserve(keylen);
    for (size_t i = 0; i < keylen; i++) {
        key.push_back(get_single_byte_xor_key(ct_blocks[i]));    
    }

    // decrypt the ct using repeating key xor with that key
    void_repeating_xor(ct_bytes_copy, key);
    std::cout << "Key is: " << bytes_to_ascii(key) << '\n';
    std::cout << "Plaintext is: " << bytes_to_ascii(ct_bytes_copy) << '\n';

    // --- Exit code ---

    return 0;
}

// --- Implementations ---

std::multimap<float, std::tuple<int, int>> find_best_keylen(const std::vector<BYTE>& ct) {

    std::multimap<float, std::tuple<int, int>> ranked_pairs;  
    const unsigned int MIN_KEYLEN = 2;
    const unsigned int MAX_KEYLEN = 40;
    const unsigned int MIN_BLOCKS = 10;
    const unsigned int MAX_BLOCKS = 20;

    for (auto keylen = MIN_KEYLEN; keylen <= ct.size() && keylen <= MAX_KEYLEN; keylen++) {
         
         unsigned int local_max_blocks = ct.size() / keylen;
         if (local_max_blocks < MIN_BLOCKS) {break;}
         

         for (unsigned int blocks = MIN_BLOCKS; blocks <= std::min(local_max_blocks, MAX_BLOCKS); blocks++) {
            
            float tmp_sum = 0.0f;
            int comparisons = 0; 

            for (unsigned int i = 0; i < blocks - 1; i++) {
                auto block_i = std::vector<unsigned char>(
                    ct.begin() + i * keylen, ct.begin() + (i + 1) * keylen
                );
                for (unsigned int j = i + 1; j < blocks; j++) {
                    auto block_j = std::vector<unsigned char>(
                        ct.begin() + j * keylen, ct.begin() + (j + 1) * keylen
                    );
                    tmp_sum += (float) hamming_distance(block_i, block_j) / (float) keylen; // remember to normalize hamming distance to key length
                    comparisons++;
                }
            }

            if (comparisons > 0) {
                float avg = tmp_sum / static_cast<float>(comparisons);
                ranked_pairs.insert({avg, std::make_tuple(keylen, blocks)});
            }
        }
    }
    
    return ranked_pairs;
} 
