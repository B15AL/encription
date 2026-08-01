#include <iostream>
#include <iomanip>
#include <array>
#include "usekey.h"
#include "aes.h"

key my_key;

void print_hex(const std::string& label, const std::array<uint8_t, 16>& block) {
    std::cout << label << ": ";
    for (uint8_t b : block) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
    std::cout << std::dec << "\n";
}

int main() {
    // Standard NIST AES-128 Test Vector
    std::string hexKey = "2b7e151628aed2a6abf7158809cf4f3c";
    std::array<uint8_t, 16> plaintext = {
        0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
        0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
    };

    if (!my_key.loadInputKey(hexKey)) {
        std::cerr << "Invalid key format!" << std::endl;
        return 1;
    }

    std::array<uint8_t, 16> state = plaintext;
    
    print_hex("Plaintext ", state);

    encript(state);
    print_hex("Ciphertext", state);

    decrypt(state);
    print_hex("Decrypted ", state);

    return 0;
}
