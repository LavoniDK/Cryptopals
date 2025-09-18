#include <iostream>
#include <vector> 
#include <iostream>
#include <fstream>
#include <sstream>

#include "ascii.h"
#include "base64.h"
#include "hex.h"
#include "linalg.h"
#include "xor.h"

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>

using BYTE = unsigned char;

int main() {
    
    const int BLOCK_SIZE = 16;
    
    // key and IV
    std::vector<BYTE> key = ascii_to_bytes("YELLOW SUBMARINE");
    std::vector<BYTE> iv = std::vector<BYTE>();
    for (int i = 0; i < BLOCK_SIZE; i++) {iv.push_back(static_cast<BYTE>(0x00));}
    
    // read ciphertext
    std::ifstream input_file("10.txt");
    std::string b64_data;
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    b64_data = buffer.str();
    input_file.close();
    
    // strip whitespace
    b64_data.erase(std::remove_if(b64_data.begin(), b64_data.end(), [](unsigned char c){ return std::isspace(c); }), b64_data.end());

    // base64 decode
    std::vector<BYTE> ct_bytes = base64_to_bytes(b64_data);

    // split into blocks
    std::vector<std::vector<BYTE>> ct_blocks = split_vector(ct_bytes, BLOCK_SIZE);  
    
    // setup AES-ECB
    CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption dec;
    dec.SetKey(key.data(), key.size());
    std::vector<BYTE> prev_block = iv;
    std::vector<std::vector<BYTE>> plaintext;

    // apply cipher for each block
    for (auto& block : ct_blocks) {
        // decrypto block with ECB
        std::vector<BYTE> decrypted(block.size());
        dec.ProcessData(decrypted.data(), block.data(), block.size());

        // append xored block
        plaintext.push_back(fixed_xor(decrypted, prev_block));

        // update
        prev_block = block;
    }


    // remove PKCS#7 padding
    BYTE pad = plaintext.back().back();
    if (pad > 0 && pad <= BLOCK_SIZE) {
        plaintext.back().resize(BLOCK_SIZE - pad);
    }

    // --- OUTPUT ---
    std::cout << bytes_to_ascii(flatten_matrix(plaintext)) << std::endl;

    // --- EXIT STATUS

    return 0;

}
