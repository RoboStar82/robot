
#pragma once

#include <Arduino.h>

class TEA {
   public:
    bool encryptData(uint8_t* data);
    bool decryptData(uint8_t* data);
    void encryptBlock(uint32_t* block);
    void decryptBlock(uint32_t* block);

   protected:
    uint32_t count = 32;
    uint32_t delta = 0x00;
    uint32_t sumDelta = 32 * 0x00;
    uint32_t key[4] = {0x00, 0x00, 0x00, 0x00};
};

extern TEA tea;
