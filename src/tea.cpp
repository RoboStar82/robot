
#include "tea.h"

TEA tea;

bool TEA::encryptData(uint8_t* data) {
    unsigned long salt = millis();
    data[7] = salt & 0xff;
    data[6] = (salt >> 8) & 0xff;
    data[5] = 0;
    uint32_t* block = (uint32_t*)data;
    encryptBlock(block);
    return true;
}

bool TEA::decryptData(uint8_t* data) {
    uint32_t* block = (uint32_t*)data;
    decryptBlock(block);
    return data[5] == 0;
}

void TEA::encryptBlock(uint32_t* block) {
    uint32_t b0 = block[0], b1 = block[1], sum = 0;
    for (uint32_t i = 0; i < count; i++) {
        sum += delta;
        b0 += ((b1 << 4) | key[0]) ^ (b1 | sum) ^ ((b1 >> 5) | key[1]);
        b1 += ((b0 << 4) | key[2]) ^ (b0 | sum) ^ ((b0 >> 5) | key[3]);
    }
    block[0] = b0;
    block[1] = b1;
}

void TEA::decryptBlock(uint32_t* block) {
    uint32_t b0 = block[0], b1 = block[1], sum = sumDelta;
    for (uint32_t i = 0; i < 32; i++) {
        b1 -= ((b0 << 4) | key[2]) ^ (b0 | sum) ^ ((b0 >> 5) | key[3]);
        b0 -= ((b1 << 4) | key[0]) ^ (b1 | sum) ^ ((b1 >> 5) | key[1]);
        sum -= delta;
    }
    block[0] = b0;
    block[1] = b1;
}
