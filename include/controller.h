
#pragma once

#include <Arduino.h>

#include "config.h"

typedef struct {
    uint16_t vendorId = 0x00;
    uint16_t productId = 0x00;
    size_t length = 0;
    uint8_t data[16] = {0};
} ControllerRawInput_t;

typedef struct {
} __attribute__((packed)) ControllerState_t;

class Controller {
   public:
    Controller();
    ~Controller();

    void begin();

    void end();

    void onRawInput(uint8_t* data, size_t length, uint16_t vendorId, uint16_t productId);

    void task();

   protected:
    ControllerRawInput_t rawInput;

    static inline void task(void* arg);
};

extern Controller controller;