#include <iostream>
#include <fstream>
#include <array>
#include <random>
#include <string>
#include <cstring>
#include "usekey.h"
#include "aes.h"

extern "C" {

  struct File_path_str{
    char** paths;
    size_t length;
    size_t capacity;
  };
  File_path_str file_find(const char* dir , const char* ext);
  void free_file_paths(File_path_str abcd);
}


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
              << "  Encrypt: " << prog_name << " -e <dir name> <file type> <32_hex_char_key>\n"
              << "  Decrypt: " << prog_name << " -d <input file> <output file > <32_hex_char_key>\n\n"
              << "Example:\n"
              << "  " << prog_name << " -e dir pdf 2b7e151628aed2a6abf7158809cf4f3c\n"
              << "  " << prog_name << " -d enc.txt new.txt 2b7e151628aed2a6abf7158809cf4f3c\n";
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
        std::cout << "[+] Encrypting: " << input_path << " with file extension= " << output_path << "\n";
        File_path_str result = file_find(input_path.c_str() , output_path.c_str());

        std::cout<<"found" << result.length<<"matching files: \n";

        std::string new_file;
        for(size_t i=0 ; i<result.length;++i){
          std::string path(result.paths[i]);
          std::cout<< "-> "<< path <<"\n";
        

          new_file = std::to_string(i)+"." + output_path;
        // Generate fresh IV
        std::array<uint8_t, 16> iv = generate_random_iv();

        // Write IV as a 16-byte header in the ciphertext file
        std::ofstream outfile(new_file, std::ios::binary);
        if (!outfile.is_open()) {
            std::cerr << "[-] Error: Could not open output file.\n";
            return 1;
        }
        outfile.write(reinterpret_cast<char*>(iv.data()), 16);
        outfile.close();

        // Append encrypted payload (using append mode in encrypt_file logic or standard stream write)
        if (encrypt_file(path, new_file, iv)) {
            std::cout << "[+] File encrypted successfully!\n";
        } else {
            std::cerr << "[-] Encryption failed.\n";
            return 1;
        }
        }
        free_file_paths(result);
        return 0 ;

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
