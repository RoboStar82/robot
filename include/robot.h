
#pragma once

#include <Arduino.h>

#include "motor.h"

class Robot {
   public:
    void begin();

    void setSpeed(int8_t speedLF, int8_t speedRF, int8_t speedLB, int8_t speedRB);

    void updateSpeed();

   protected:
#if ROBOT_HAS_MOTOR_PWM && ROBOT_HAS_MOTOR_ENCODER
    MotorPWM motorLF = MotorPWM(4, 5, 4, 5);
    MotorPWM motorRF = MotorPWM(1, 2, 1, 2);
    MotorPWM motorLB = MotorPWM(6, 7, 6, 7);
    MotorPWM motorRB = MotorPWM(42, 41, 42, 41);
#elif ROBOT_HAS_MOTOR_PWM
    MotorPWM motorLF = MotorPWM(4, 5);
    MotorPWM motorRF = MotorPWM(1, 2);
    MotorPWM motorLB = MotorPWM(6, 7);
    MotorPWM motorRB = MotorPWM(42, 41);
#elif ROBOT_HAS_MOTOR_ENCODER
    MotorEncoder motorLF = MotorEncoder(4, 5);
    MotorEncoder motorRF = MotorEncoder(1, 2);
    MotorEncoder motorLB = MotorEncoder(6, 7);
    MotorEncoder motorRB = MotorEncoder(42, 41);
#else
    Motor motorLF;
    Motor motorRF;
    Motor motorLB;
    Motor motorRB;
#endif
};

extern Robot robot;
