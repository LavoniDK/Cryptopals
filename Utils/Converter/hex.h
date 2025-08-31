#ifndef HEX_H
#define HEX_H

#include <string>
#include <vector>

using BYTE = unsigned char;

// --- Function Prototypes ---
std::string bytes_to_hex(const std::vector<BYTE>& bytes);
std::vector<BYTE> hex_to_bytes(const std::string& hex);

#endif 
