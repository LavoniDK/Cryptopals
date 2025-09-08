#include <vector>
#include <string>
#include <stdexcept>

#include "linalg.h"

using BYTE = unsigned char;

// --- Function Implementations ---
int hamming_distance(const std::vector<BYTE>& bytes1, const std::vector<BYTE>& bytes2) { 
    
    int dist = 0;
    if (bytes1.size() != bytes2.size()) {throw std::invalid_argument("string must be of equal length");}

    for (size_t i = 0; i < bytes1.size(); i++) {
        for (unsigned val = bytes1[i] ^ bytes2[i]; val > 0; dist++) {
            val = val & (val - 1); 
        }
    }
    return dist;
}

void void_transpose(std::vector<std::vector<BYTE>>& matrix) {
    
    std::vector<std::vector<BYTE>> trans_vec(matrix[0].size(), std::vector<BYTE>());

    for (unsigned int i = 0; i < matrix.size(); i++)
    {
        for (unsigned int j = 0; j < matrix[i].size(); j++)
        {
            trans_vec[j].push_back(matrix[i][j]);
        }
    }
    matrix = trans_vec;
}

std::vector<std::vector<BYTE>> transpose(const std::vector<std::vector<BYTE>>& matrix) {
    
    std::vector<std::vector<BYTE>> trans_vec(matrix[0].size(), std::vector<BYTE>());

    for (unsigned int i = 0; i < matrix.size(); i++)
    {
        for (unsigned int j = 0; j < matrix[i].size(); j++)
        {
            trans_vec[j].push_back(matrix[i][j]);
        }
    }
    return trans_vec;
}

std::vector<std::vector<BYTE>> split_vector(const std::vector<BYTE>& bytes, const unsigned int keylen) {
    
    std::vector<std::vector<BYTE>> blocks;
    size_t total_blocks = (bytes.size() + keylen - 1) / keylen;
    blocks.reserve(total_blocks);

    for (size_t i = 0; i < bytes.size(); i += keylen) {
        size_t end = std::min(i + keylen, bytes.size());
        blocks.emplace_back(bytes.begin() + i, bytes.begin() + end);
    }
    return blocks;
}

// Could have been named unsplit_vector
std::vector<BYTE> flatten_matrix(const std::vector<std::vector<BYTE>>& matrix) {
    std::vector<BYTE> vec;
    if (!matrix.empty()) {
        vec.reserve((matrix.size() - 1) * matrix[0].size()); // padding not ensured
    }
    for (const auto& row : matrix) {
        vec.insert(vec.end(), row.begin(), row.end());
    }
    return vec;
}

