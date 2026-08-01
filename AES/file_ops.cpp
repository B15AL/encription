#include <fstream>
#include <vector>
#include <array>
#include <iostream>
#include "aes.h"

// CBC Encryption for Files
bool encrypt_file(const std::string& input_path, const std::string& output_path, const std::array<uint8_t, 16>& iv) {
    std::ifstream infile(input_path, std::ios::binary);
    // MUST use std::ios::app here so we don't overwrite the IV header written by main.cpp
    std::ofstream outfile(output_path, std::ios::binary | std::ios::app);

    if (!infile.is_open() || !outfile.is_open()) {
        std::cerr << "[-] Failed to open file streams." << std::endl;
        return false;
    }

    std::array<uint8_t, 16> prev_cipher = iv;
    std::array<uint8_t, 16> block;

    while (infile.good()) {
        infile.read(reinterpret_cast<char*>(block.data()), 16);
        std::streamsize bytes_read = infile.gcount();

        if (bytes_read == 0) break; // Avoid trailing empty writes

        // Handle PKCS#7 Padding on the final block
        if (bytes_read < 16) {
            uint8_t pad_val = 16 - bytes_read;
            for (size_t i = bytes_read; i < 16; ++i) {
                block[i] = pad_val;
            }

            // CBC XOR with previous ciphertext
            for (int i = 0; i < 16; ++i) block[i] ^= prev_cipher[i];

            encript(block);
            outfile.write(reinterpret_cast<char*>(block.data()), 16);
            break;
        }

        // Full block CBC XOR
        for (int i = 0; i < 16; ++i) block[i] ^= prev_cipher[i];

        encript(block);
        outfile.write(reinterpret_cast<char*>(block.data()), 16);
        prev_cipher = block;

        // If file ended exactly on a 16-byte boundary, write an entire block of padding
        if (infile.peek() == EOF) {
            std::array<uint8_t, 16> pad_block;
            pad_block.fill(16); // 16 bytes of 0x10
            for (int i = 0; i < 16; ++i) pad_block[i] ^= prev_cipher[i];
            encript(pad_block);
            outfile.write(reinterpret_cast<char*>(pad_block.data()), 16);
            break;
        }
    }

    return true;
}

// CBC Decryption for Files
bool decrypt_file(const std::string& input_path, const std::string& output_path, const std::array<uint8_t, 16>& iv) {
    std::ifstream infile(input_path, std::ios::binary | std::ios::ate);
    // Overwrite the output file cleanly during decryption
    std::ofstream outfile(output_path, std::ios::binary);

    if (!infile.is_open() || !outfile.is_open()) {
        std::cerr << "[-] Failed to open file streams." << std::endl;
        return false;
    }

    std::streamsize file_size = infile.tellg();
    if (file_size < 32 || (file_size - 16) % 16 != 0) {
        std::cerr << "[-] Invalid ciphertext file size." << std::endl;
        return false;
    }

    infile.seekg(16, std::ios::beg);
    size_t total_blocks = (file_size - 16) / 16;

    std::array<uint8_t, 16> prev_cipher = iv;
    std::array<uint8_t, 16> cipher_block;
    std::array<uint8_t, 16> plain_block;

    for (size_t b = 0; b < total_blocks; ++b) {
        infile.read(reinterpret_cast<char*>(cipher_block.data()), 16);
        std::array<uint8_t, 16> current_cipher = cipher_block;

        decrypt(cipher_block);

        // CBC XOR to obtain original plaintext
        for (int i = 0; i < 16; ++i) {
            plain_block[i] = cipher_block[i] ^ prev_cipher[i];
        }
        prev_cipher = current_cipher;

        // Strip PKCS#7 padding on the last block
        if (b == total_blocks - 1) {
            uint8_t pad_val = plain_block[15];
            if (pad_val > 0 && pad_val <= 16) {
                outfile.write(reinterpret_cast<char*>(plain_block.data()), 16 - pad_val);
            } else {
                std::cerr << "[-] Invalid padding detected during decryption." << std::endl;
                return false;
            }
        } else {
            outfile.write(reinterpret_cast<char*>(plain_block.data()), 16);
        }
    }

    return true;
}
