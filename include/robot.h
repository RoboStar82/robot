
#pragma once

#include <Arduino.h>

#include "motor.h"

class Robot {
   public:
    void begin();

    void setSpeed(int speedLF, int speedRF, int speedLB, int speedRB);

    void updateSpeed();
};

extern Robot robot;
