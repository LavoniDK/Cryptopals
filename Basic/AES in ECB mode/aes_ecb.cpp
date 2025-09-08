#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

#include "ascii.h"
#include "base64.h"

using BYTE = unsigned char;

int main() {

    // --- DATA ---

    std::string ascii_key = "YELLOW SUBMARINE";
    std::vector<BYTE> key = ascii_to_bytes(ascii_key);

    std::ifstream input_file("7.txt");
    std::string b64_data;
    std::stringstream buffer;
    buffer << input_file.rdbuf();
    b64_data = buffer.str();
    b64_data.erase(std::remove_if(b64_data.begin(), b64_data.end(), [](unsigned char c){ return std::isspace(c); }), b64_data.end());

    input_file.close();
    std::vector<BYTE> ct_bytes = base64_to_bytes(b64_data);

    // --- DECRYPT

    std::vector<BYTE> plaintext(ct_bytes.size() + CryptoPP::AES::BLOCKSIZE);
    CryptoPP::ArraySink arrsink(plaintext.data(), plaintext.size());

    CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption dec;
    dec.SetKey(key.data(), key.size());

    CryptoPP::ArraySource(
        ct_bytes.data(), ct_bytes.size(), true,
        new CryptoPP::StreamTransformationFilter(
            dec,
            new CryptoPP::Redirector(arrsink),
            CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING
        )
    );

    plaintext.resize(arrsink.TotalPutLength());

    // --- OUTPUT ---
    std::cout << bytes_to_ascii(plaintext) << std::endl;

    // --- EXIT STATUS
    
    return 0;

}
