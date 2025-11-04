
#pragma once

#include <Arduino.h>

class Motor {
   public:
    void setSpeed(int value);
};

class MotorEncoder : Motor {
   public:
    MotorEncoder(uint8_t encoderPin1, uint8_t encoderPin2);

    void setSpeed(int value);

   protected:
    uint8_t encoderPin1 = 0;
    uint8_t encoderPin2 = 0;
};

class MotorPWM : MotorEncoder {
   public:
    MotorPWM(uint8_t pwmPin1, uint8_t pwmPin2);
    MotorPWM(uint8_t pwmPin1, uint8_t pwmPin2, uint8_t encoderPin1, uint8_t encoderPin2);

    virtual void setSpeed(int value);

   protected:
    uint8_t pwmPin1 = 0;
    uint8_t pwmPin2 = 0;
};
