#ifndef BASE64_H
#define BASE64_H

#include <string>
#include <vector>

using BYTE = unsigned char;

// --- Function Prototypes ---
std::string bytes_to_base64(const std::vector<BYTE>& bytes, bool enable_padding = true);
std::vector<BYTE> base64_to_bytes(const std::string& b64);

#endif 
