#ifndef LINALG
#define LINALG

#include <string>
#include <vector>

using BYTE = unsigned char;

// --- Function Prototypes ---
int hamming_distance(const std::vector<BYTE>& bytes1, const std::vector<BYTE>& bytes2);

std::vector<std::vector<BYTE>> split_vector(const std::vector<BYTE>& bytes, const unsigned int keylen);
std::vector<BYTE> flatten_matrix(const std::vector<std::vector<BYTE>>& matrix);

void void_transpose(std::vector<std::vector<BYTE>>& matrix);
std::vector<std::vector<BYTE>> transpose(const std::vector<std::vector<BYTE>>& matrix);

#endif 
