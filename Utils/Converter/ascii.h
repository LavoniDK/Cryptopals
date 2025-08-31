#ifndef ASCII_H
#define ASCII_H

#include <string>
#include <vector>

using BYTE = unsigned char;

// --- Function Prototypes ---
std::string bytes_to_ascii(const std::vector<BYTE>& bytes);
std::vector<BYTE> ascii_to_bytes(const std::string& ascii);

#endif
