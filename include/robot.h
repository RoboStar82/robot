
#pragma once

#ifndef DEBUG_CONTROL
#define DEBUG_CONTROL 1
#endif

#ifndef DEBUG_MOTOR
#define DEBUG_MOTOR 1
#endif

#include <Arduino.h>
#include <Wire.h>

#if ROBOT_HAS_LED
#include <Adafruit_NeoPixel.h>
#endif

#include "ble.h"
#include "bmx.h"
#include "lidar.h"
#include "motor.h"
#include "stick.h"
#include "print.h"

class Robot {
   public:
    Controller *controller = nullptr;

    Motor *motorLF = nullptr;
    Motor *motorRF = nullptr;
    Motor *motorLB = nullptr;
    Motor *motorRB = nullptr;
    Motor *motors[4] = {nullptr, nullptr, nullptr, nullptr};

    Stick *L = nullptr;
    Stick *R = nullptr;
    Stick *D = nullptr;

    bool changeXY = false;
    char autoMode = 0;

    void setMotors(Motor *motorLF, Motor *motorRF, Motor *motorLB, Motor *motorRB);

    void setController(Controller *controller);

    void setSticks(Stick *L, Stick *R, Stick *D);

#if ROBOT_HAS_LED
    Adafruit_NeoPixel led = Adafruit_NeoPixel(4, 16, NEO_GRB + NEO_KHZ800);
#endif

    void updateSpeed();

    void loop();
};

void robotSetup();

void robotBegin(void *params);

void robotLoop();
