#include <iostream>
#include <array>
#include "usekey.h"
#include "aes.h"
extern key my_key; // Uses the same key instance

// --- Inverse SubBytes ---
std::array<uint8_t, 16> invSubBytes(std::array<uint8_t, 16> state) {
    for (int i = 0; i < 16; i++) {
        state[i] = my_key.inv_sbox[state[i]];
    }
    return state;
}

// --- Inverse ShiftRows ---
std::array<uint8_t, 16> invShiftRow(std::array<uint8_t, 16> state) {
    std::array<uint8_t, 16> temp = state;

    // Row 0 (indices 0, 4, 8, 12) -> No shift

    // Row 1 (indices 1, 5, 9, 13) -> Shift right by 1
    state[1]  = temp[13];
    state[5]  = temp[1];
    state[9]  = temp[5];
    state[13] = temp[9];

    // Row 2 (indices 2, 6, 10, 14) -> Shift right by 2
    state[2]  = temp[10];
    state[6]  = temp[14];
    state[10] = temp[2];
    state[14] = temp[6];

    // Row 3 (indices 3, 7, 11, 15) -> Shift right by 3
    state[3]  = temp[7];
    state[7]  = temp[11];
    state[11] = temp[15];
    state[15] = temp[3];

    return state;
}

// --- GF(2^8) Galois Field Arithmetic Helpers ---
inline uint8_t multy_by_2(uint8_t x) {
    bool temp = x & 0x80;
    x = x << 1;
    if (temp) {
        x ^= 0x1B;
    }
    return x;
}

uint8_t mult_gf(uint8_t x, uint8_t y) {
    uint8_t result = 0;
    uint8_t temp = x;
    while (y > 0) {
        if (y & 1) {
            result ^= temp;
        }
        temp = multy_by_2(temp);
        y >>= 1;
    }
    return result;
}

// Multiplications required for Inverse MixColumns
uint8_t multy_by_9(uint8_t x)  { return mult_gf(x, 0x09); }
uint8_t multy_by_11(uint8_t x) { return mult_gf(x, 0x0B); }
uint8_t multy_by_13(uint8_t x) { return mult_gf(x, 0x0D); }
uint8_t multy_by_14(uint8_t x) { return mult_gf(x, 0x0E); }

// --- Inverse MixColumns ---
std::array<uint8_t, 16> invMixColumn(std::array<uint8_t, 16> state) {
    std::array<uint8_t, 16> b;

    for (int i = 0; i < 16; i += 4) {
        b[0 + i] = multy_by_14(state[0 + i]) ^ multy_by_11(state[1 + i]) ^ multy_by_13(state[2 + i]) ^ multy_by_9(state[3 + i]);
        b[1 + i] = multy_by_9(state[0 + i])  ^ multy_by_14(state[1 + i]) ^ multy_by_11(state[2 + i]) ^ multy_by_13(state[3 + i]);
        b[2 + i] = multy_by_13(state[0 + i]) ^ multy_by_9(state[1 + i])  ^ multy_by_14(state[2 + i]) ^ multy_by_11(state[3 + i]);
        b[3 + i] = multy_by_11(state[0 + i]) ^ multy_by_13(state[1 + i]) ^ multy_by_9(state[2 + i])  ^ multy_by_14(state[3 + i]);
    }

    return b;
}

// --- Main Decrypt Function ---
void decrypt(std::array<uint8_t, 16>& state) {
    int round = 10;

    // Initial Round (Reverse of Encryption's Final Round)
    state = add_key(state, round);
    state = invShiftRow(state);
    state = invSubBytes(state);
    round--;

    // Main Rounds 9 down to 1
    while (round > 0) {
        state = add_key(state,round);
        state = invMixColumn(state);
        state = invShiftRow(state);
        state = invSubBytes(state);
        round--;
    }

    // Final Round 0
    state = add_key(state, round);
}
