#ifndef AES_H
#define AES_H

#include <array>
#include <cstdint>

std::array<uint8_t, 16> add_key(std::array<uint8_t, 16> state, int r);
void encript(std::array<uint8_t, 16>& state);
void decrypt(std::array<uint8_t, 16>& state);

#endif
