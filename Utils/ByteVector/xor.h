#ifndef XOR
#define XOR

#include <vector>
#include <string>
#include <map>     
#include <stdexcept> 

using BYTE = unsigned char;

// --- Prototypes ---
std::vector<BYTE> fixed_xor(const std::vector<BYTE>& bytes1, const std::vector<BYTE>& bytes2);

std::vector<BYTE> repeating_xor(const std::vector<BYTE>& bytes, const std::vector<BYTE>& key);
void void_repeating_xor(std::vector<BYTE>& bytes, const std::vector<BYTE>& key);

std::vector<BYTE> constant_xor(const std::vector<BYTE>& bytes, const BYTE key);
void void_constant_xor(std::vector<BYTE>& bytes, const BYTE key);

BYTE get_single_byte_xor_key(const std::vector<BYTE>& ct_bytes);
double chi_squared(const std::vector<BYTE>& bytes);
//std::vector<BYTE> get_repeating_xor_key(const std::vector<BYTE>& ct)

#endif 

