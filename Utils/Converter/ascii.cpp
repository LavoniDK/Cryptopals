#include <vector>
#include <string>

#include "ascii.h"

// --- Function Implementations ---
std::string bytes_to_ascii(const std::vector<BYTE>& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

std::vector<BYTE> ascii_to_bytes(const std::string& ascii) {
    return std::vector<BYTE>(ascii.begin(), ascii.end());
}
