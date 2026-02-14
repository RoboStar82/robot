
#pragma once

#include <Arduino.h>

#include "rotaryDecoder.h"

class Encoder {
   public:
    Encoder(int8_t i2cAddress, int8_t interruptPin);
    ~Encoder();
    void begin();

    void task();
    static void task(void* arg);
    static void interrupt();

    bool needUpdate = false;

   protected:
    int8_t i2cAddress = 0;
    int8_t interruptPin = 0;
    rotaryDecoder* decoder = nullptr;
};

#ifndef ROBOT_MOTOR_ENCODER_I2C_ADDRESS
#define ROBOT_MOTOR_ENCODER_I2C_ADDRESS 0x20
#endif

#ifndef ROBOT_MOTOR_ENCODER_INTERRUPT_PIN
#define ROBOT_MOTOR_ENCODER_INTERRUPT_PIN 0
#endif

extern Encoder encoder;
