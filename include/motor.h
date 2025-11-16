
#pragma once

#include <Arduino.h>

class Motor {
   public:
    Motor(const char* name);
    void begin();

    void setSpeed(int value);
    void setMinSpeed(uint value);
    void setMaxSpeed(uint value);

   protected:
    const char* name;
};

class MotorEncoder : Motor {
   public:
    MotorEncoder(const char* name, uint8_t encoderPin1, uint8_t encoderPin2);

    void begin();

    void setSpeed(int value);
    void setMinSpeed(uint value);
    void setMaxSpeed(uint value);

   protected:
    const char* name;
    uint8_t encoderPin1 = 0;
    uint8_t encoderPin2 = 0;
};

class MotorPWM : MotorEncoder {
   public:
    MotorPWM(const char* name, uint8_t pwmPin1, uint8_t pwmPin2);
    MotorPWM(const char* name, uint8_t pwmPin1, uint8_t pwmPin2, uint8_t encoderPin1, uint8_t encoderPin2);

    void begin();

    virtual void setSpeed(int value);
    void setMinSpeed(uint value);
    void setMaxSpeed(uint value);

   protected:
    const char* name;
    uint8_t pwmPin1 = 0;
    uint8_t pwmPin2 = 0;
    uint minSpeed = 50;
    uint maxSpeed = 200;
};
