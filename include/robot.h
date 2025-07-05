
#pragma once

#include <Arduino.h>
#include <Wire.h>

#include "ble.h"
#include "bmx.h"
#include "lidar.h"
#include "motor.h"
#include "print.h"

class Robot {
   public:
    Controller *controller = nullptr;

    Motor *motorLF = nullptr;
    Motor *motorRF = nullptr;
    Motor *motorLB = nullptr;
    Motor *motorRB = nullptr;
    Motor *motors[4] = {nullptr, nullptr, nullptr, nullptr};

    char autoMode = 0;

    // Состояние моторов

    bool healthLF = false;
    bool healthRF = false;
    bool healthLB = false;
    bool healthRB = false;

    // Скорости моторов

    // Left front
    int speedLF = 0;
    // Right front
    int speedRF = 0;
    // Left back
    int speedLB = 0;
    // Right back
    int speedRB = 0;

    int motorMinSpeed = 20;
    int motorMaxSpeed = 80;

    int speedD = 5;
    int speedD2 = 2;

    void setMotors(Motor *motorLF, Motor *motorRF, Motor *motorLB, Motor *motorRB);

    void setController(Controller *controller);

    void updateSpeed();

    void loop();
};

void robotSetup();

void robotBegin(void *params);

void robotLoop();
