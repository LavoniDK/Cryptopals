# include <iostream>
# include <exception>
# include <vector>
# include <tuple>
# include <map>
typedef unsigned char BYTE; 

// --- Prototypes ---
int hamming_dist(const std::vector<unsigned char>& b1,
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



// Returns a map of tuples containing <keysize, blockcount> sorted ascendingly on their pairwise average normalized hamming distance.
std::multimap<float, tuple<int, int, float>> best_keylen_and_blockcount(const std::vector<unsigned char>& ciphertext, int blocks) {

    std::multimap<float, tuple<int, int>> ranked_pairs;  

    for (int keylen = 1; key_len <= ciphertext.size(); key_len++) {
         int max_blocks = ciphertext.size() / keylen;
       
         for (int blocks = 2; blocks <= max_blocks; blocks++) {
            float tmp_sum = 0.0f;
            int comparisons = 0; 

            for (int i = 0; i < blocks - 1; i++) {
                auto block_i = std::vector<unsigned char>(
                    ciphertext.begin() + i * key_len, ciphertext.begin() + (i + 1) * key_len
                );
                for (int j = i + 1; j < blocks; j++) {
                    auto block_j = std::vector<unsigned char>(
                        ciphertext.begin() + j * key_len, ciphertext.begin() + (j + 1) * key_len
                    );
                    tmp_sum += (float) hamming_distance(block_i, block_j) / (float) keylen; // remember to normalize hamming distance to key length
                    comparisons++;
                }
            }

            if (comparisons > 0) {
                float avg = tmp_sum / (float) comparisons;
                ranked_pairs.insert({avg, std::make_tuple(keylen, blocks)});
            }
        }
    }
    
    return ranked_pairs;
} 

int hamming_dist(const std::vector<unsigned char>& b1, const std::vector<unsigned char>& b2) { 
    
    int dist = 0;
    if (b1.size() != b2.size()) {throw std::invalid_argument("string must be of equal length");}

    for (size_t i = 0; i < b1.size(); i++) {
        for (unsigned val = b1[i] ^ b2[i]; val > 0; dist++) {
            val = val & (val - 1); 
        }
    }
    
    return dist;
}
