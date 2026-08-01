#include <iostream>
#include <fstream>
#include <array>
#include <random>
#include <string>
#include <cstring>
#include "usekey.h"
#include "aes.h"

key my_key;

// Generate a pseudo-random IV for CBC mode
std::array<uint8_t, 16> generate_random_iv() {
    std::array<uint8_t, 16> iv;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);

    for (int i = 0; i < 16; ++i) {
        iv[i] = static_cast<uint8_t>(dis(gen));
    }
    return iv;
}

void print_usage(const char* prog_name) {
    std::cout << "AES-128 CBC File Encryption Tool\n\n"
              << "Usage:\n"
              << "  Encrypt: " << prog_name << " -e <input_file> <output_file> <32_hex_char_key>\n"
              << "  Decrypt: " << prog_name << " -d <input_file> <output_file> <32_hex_char_key>\n\n"
              << "Example:\n"
              << "  " << prog_name << " -e secret.pdf secret.pdf.enc 2b7e151628aed2a6abf7158809cf4f3c\n"
              << "  " << prog_name << " -d secret.pdf.enc secret_dec.pdf 2b7e151628aed2a6abf7158809cf4f3c\n";
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        print_usage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];
    std::string input_path = argv[2];
    std::string output_path = argv[3];
    std::string hexKey = argv[4];

    // Load key
    if (!my_key.loadInputKey(hexKey)) {
        std::cerr << "[-] Error: Key must be exactly 32 hex characters (128 bits).\n";
        return 1;
    }

    if (mode == "-e") {
        std::cout << "[+] Encrypting: " << input_path << " -> " << output_path << "\n";
        
        // Generate fresh IV
        std::array<uint8_t, 16> iv = generate_random_iv();

        // Write IV as a 16-byte header in the ciphertext file
        std::ofstream outfile(output_path, std::ios::binary);
        if (!outfile.is_open()) {
            std::cerr << "[-] Error: Could not open output file.\n";
            return 1;
        }
        outfile.write(reinterpret_cast<char*>(iv.data()), 16);
        outfile.close();

        // Append encrypted payload (using append mode in encrypt_file logic or standard stream write)
        if (encrypt_file(input_path, output_path, iv)) {
            std::cout << "[+] File encrypted successfully!\n";
        } else {
            std::cerr << "[-] Encryption failed.\n";
            return 1;
        }

    } else if (mode == "-d") {
        std::cout << "[+] Decrypting: " << input_path << " -> " << output_path << "\n";

        // Read the 16-byte IV header from the ciphertext file
        std::ifstream infile(input_path, std::ios::binary);
        if (!infile.is_open()) {
            std::cerr << "[-] Error: Could not open input file.\n";
            return 1;
        }

        std::array<uint8_t, 16> iv;
        infile.read(reinterpret_cast<char*>(iv.data()), 16);
        
        if (infile.gcount() < 16) {
            std::cerr << "[-] Error: File is too small to contain a valid IV header.\n";
            return 1;
        }
        infile.close();

        if (decrypt_file(input_path, output_path, iv)) {
            std::cout << "[+] File decrypted successfully!\n";
        } else {
            std::cerr << "[-] Decryption failed.\n";
            return 1;
        }

    } else {
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
