// ============================================= Base64 Handler ======================================================

#ifndef BASE64_H
#define BASE64_H
#include <Arduino.h>

int base64_dec_len(const char *input, int inputLen) {
    int padding = 0;
    if (input[inputLen - 1] == '=') padding++;
    if (input[inputLen - 2] == '=') padding++;
    return (inputLen * 3) / 4 - padding;
}

int base64_decode(char *output, const char *input, int inputLen) {
    const int8_t lookup[] = {
        62, -1, -1, -1, 63, // + and /
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // 0-9
        -1, -1, -1, -2, -1, -1, -1, // padding and symbols
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, // A-O
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // P-Z
        -1, -1, -1, -1, -1, -1, // symbols
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, // a-o
        42, 43, 44, 45, 46, 47, 48, 49, 50, 51 // p-z
    };

    int buffer = 0;
    int bits_collected = 0;
    int outIndex = 0;

    for (int i = 0; i < inputLen; ++i) {
        char ch = input[i];
        int val;

        if (ch < '+' || ch > 'z') continue;

        val = lookup[ch - '+'];
        if (val < 0) continue;

        buffer = (buffer << 6) | val;
        bits_collected += 6;

        if (bits_collected >= 8) {
            bits_collected -= 8;
            output[outIndex++] = (char)((buffer >> bits_collected) & 0xFF);
        }
    }

    return outIndex;
}

#endif
