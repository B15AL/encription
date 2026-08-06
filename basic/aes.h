#ifndef AES_H
#define AES_H

#include <array>
#include <cstdint>
#include <string>
std::array<uint8_t, 16> add_key(std::array<uint8_t, 16> state, int r);
void encript(std::array<uint8_t, 16>& state);
void decrypt(std::array<uint8_t, 16>& state);

bool encrypt_file(const std::string& input_path , const std::string& output_path, const std::array<uint8_t , 16>& iv);
bool decrypt_file(const std::string& input_path , const std::string& output_path, const std::array<uint8_t , 16>& iv);


#endif
