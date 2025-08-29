# include <iostream>
# include <exception>
# include <vector>
# include <list>

// --- Prototypes ---
int normalized_hamming_dist(const std::vector<unsigned char>& b1,
                            const std::vector<unsigned char>& b2);

std::vector<unsigned char> str_to_bytes(const std::string& str);
 
// --- Entry point ---
int main() {
    // --- Data ---
    std::string str1 = "this is a test";
    std::string str2 = "wokka wokka!!!";

    // --- Pipeline ---
    
    // convert b64 to bytes
    // for keysize k and blocks b compute the 5 largest normalized and averaged pairwise hamming distances of the first b blocks of lenght k 
    // break the ciphertext down into blocks of of size <largest key_length candidates>
    // Transpose the blocks
    // Solve the repeating-key xor for each block, the solution for each block is the char in the corresponding position in the repeating key
    // decrypt the ciphertext using repeating key xor with that key


    // --- TESTING --- 
    std::cout << normalized_hamming_dist(
                            str_to_bytes(str1),
                            str_to_bytes(str2))
              << '\n';
    return 0;
}

// --- Implementations ---

std::list<int> five_best_key_len(const std::vector<unsigned char>& ciphertext, int blocks) {
    if (blocks > ciphertext.size()) {throw std::invalid_argument("Cant have more blocks than text length");}

    int max_key_length = ciphertext.size() / blocks;
    
    int factorial(int n) {
        if (n == 0 || n == 1)
            return 1;
        return n * factorial(n - 1);
    }
    int normalization_factor = factorial(blocks); 

} 

int normalized_hamming_dist(const std::vector<unsigned char>& b1, const std::vector<unsigned char>& b2) { 
    
    int dist = 0;
    if (b1.size() != b2.size()) {throw std::invalid_argument("string must be of equal length");}

    for (size_t i = 0; i < b1.size(); i++) {
        for (unsigned val = b1[i] ^ b2[i]; val > 0; dist++) {
            val = val & (val - 1); 
        }
    }
    
    return dist;
}

std::vector<unsigned char> str_to_bytes(const std::string& str) {
    return std::vector<unsigned char>(str.begin(), str.end());
}
    






















