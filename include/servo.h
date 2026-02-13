
#pragma once

#include <Arduino.h>

class Servo {
   public:
    Servo(const char* name);
    ~Servo();

    void begin();

    const char* getName();

    void setMinAngle(float value);
    void setMaxAngle(float value);

    void setAngle(float value, bool force = false);

   protected:
    const char* name;
    float minAngle = 0.0f;
    float maxAngle = 180.0f;
    float angle = 90.0f;
};

class ServoPWM : Servo {
   public:
    ServoPWM(const char* name, uint8_t pin);
    ~ServoPWM();

    void begin();

    using Servo::getName;

    using Servo::setMaxAngle;
    using Servo::setMinAngle;

    void setAngle(float value, bool force = false);

   protected:
    using Servo::angle;
    using Servo::maxAngle;
    using Servo::minAngle;
    using Servo::name;
    uint8_t pwmPin = 0;
};

#ifndef ROBOT_SERVO_LF_PIN
#define ROBOT_SERVO_LF_PIN 18
#endif
#ifndef ROBOT_SERVO_RF_PIN
#define ROBOT_SERVO_RF_PIN 17
#endif
#ifndef ROBOT_SERVO_LB_PIN
#define ROBOT_SERVO_LB_PIN 39
#endif
#ifndef ROBOT_SERVO_RB_PIN
#define ROBOT_SERVO_RB_PIN 38
#endif

#if ROBOT_HAS_SERVO_PWM
extern ServoPWM servoLF;
extern ServoPWM servoRF;
extern ServoPWM servoLB;
extern ServoPWM servoRB;
#else
extern Servo servoLF;
extern Servo servoRF;
extern Servo servoLB;
extern Servo servoRB;
#endif

#if ROBOT_HAS_SERVO_1
extern ServoPWM servo1;
#else
extern Servo servo1;
#endif
#if ROBOT_HAS_SERVO_2
extern ServoPWM servo2;
#else
extern Servo servo2;
#endif
#if ROBOT_HAS_SERVO_3
extern ServoPWM servo3;
#else
extern Servo servo3;
#endif
#if ROBOT_HAS_SERVO_4
extern ServoPWM servo4;
#else
extern Servo servo4;
#endif
