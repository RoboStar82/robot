
#pragma once

#include <Arduino.h>

class Servo {
   public:
    Servo(const char* name);
    ~Servo();

    void begin();

    const char* getName();

    void setMinAngle(int value);
    void setMaxAngle(int value);

    void setAngle(int value, bool force = false);

   protected:
    const char* name;
    int minAngle = 0;
    int maxAngle = 180;
    int angle = 90;
};

class ServoPWM : Servo {
   public:
    ServoPWM(const char* name, uint8_t pin);
    ~ServoPWM();

    void begin();

    using Servo::getName;

    using Servo::setMaxAngle;
    using Servo::setMinAngle;

    void setAngle(int value, bool force = false);

   protected:
    using Servo::angle;
    using Servo::maxAngle;
    using Servo::minAngle;
    using Servo::name;
    uint8_t pwmPin = 0;
};

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
