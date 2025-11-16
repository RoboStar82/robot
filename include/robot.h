
#pragma once

#include <Arduino.h>

#include "motor.h"

class Robot {
   public:
    void begin();

    void setSpeed(int speedLF, int speedRF, int speedLB, int speedRB);

    void updateSpeed();

   protected:
#if ROBOT_HAS_MOTOR_PWM && ROBOT_HAS_MOTOR_ENCODER
    MotorPWM motorLF = MotorPWM("LF", 4, 5, 4, 5);
    MotorPWM motorRF = MotorPWM("RF", 2, 1, 2, 1);
    MotorPWM motorLB = MotorPWM("LB", 7, 6, 7, 6);
    MotorPWM motorRB = MotorPWM("RB", 41, 42, 41, 42);
    MotorPWM motorCC = MotorPWM("CC", 39, 40);
#elif ROBOT_HAS_MOTOR_PWM
    MotorPWM motorLF = MotorPWM("LF", 4, 5);
    MotorPWM motorRF = MotorPWM("RF", 2, 1);
    MotorPWM motorLB = MotorPWM("LB", 7, 6);
    MotorPWM motorRB = MotorPWM("RB", 41, 42);
    MotorPWM motorCC = MotorPWM("CC", 39, 40);
#elif ROBOT_HAS_MOTOR_ENCODER
    MotorEncoder motorLF = MotorEncoder("LF", 4, 5);
    MotorEncoder motorRF = MotorEncoder("RF", 2, 1);
    MotorEncoder motorLB = MotorEncoder("LB", 7, 6);
    MotorEncoder motorRB = MotorEncoder("RB", 41, 42);
    Motor motorCC = Motor("CC");
#else
    Motor motorLF = Motor("LF");
    Motor motorRF = Motor("RF");
    Motor motorLB = Motor("LB");
    Motor motorRB = Motor("RB");
    Motor motorCC = Motor("CC");
#endif
};

extern Robot robot;
