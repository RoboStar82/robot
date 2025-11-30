
#pragma once

#include <Arduino.h>

class Robot {
   public:
    Robot();
    ~Robot();

    void begin();

    void setSpeed(int speedLF, int speedRF, int speedLB, int speedRB);

    void updateSpeed();

    void updateServo();

   protected:
    int raise = 0;
};

extern Robot robot;
