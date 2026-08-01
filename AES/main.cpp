#include <iostream>
#include <array>
#include "usekey.h"
#include "aes.h"

key my_key;

int main() {
    std::string hexKey = "2b7e151628aed2a6abf7158809cf4f3c";
    
    // Initialization Vector for CBC Mode
    std::array<uint8_t, 16> iv = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    if (!my_key.loadInputKey(hexKey)) {
        std::cerr << "Failed to load key." << std::endl;
        return 1;
    }

    std::string input_file = "test.txt";
    std::string enc_file = "test.txt.enc";
    std::string dec_file = "test_dec.txt";

    std::cout << "[+] Encrypting file: " << input_file << "..." << std::endl;
    if (encrypt_file(input_file, enc_file, iv)) {
        std::cout << "[+] Encryption successful -> " << enc_file << std::endl;
    }

    std::cout << "[+] Decrypting file: " << enc_file << "..." << std::endl;
    if (decrypt_file(enc_file, dec_file, iv)) {
        std::cout << "[+] Decryption successful -> " << dec_file << std::endl;
    }

    return 0;
}
