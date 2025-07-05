
#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <iarduino_I2C_Motor.h>

#include "ble.h"
#include "bmx.h"
#include "lidar.h"
#include "print.h"

class Robot {
   public:
    Controller *controller = nullptr;

    char autoMode = 0;

    bool motorStopNeutral = true;
    uint16_t motorNominalRPM = 170;
    float motorVoltage = 12.0f;
    float motorReducer = 53.0f;
    uint8_t motorMagnet = 7;

    iarduino_I2C_Motor *motorLF = nullptr;
    iarduino_I2C_Motor *motorRF = nullptr;
    iarduino_I2C_Motor *motorLB = nullptr;
    iarduino_I2C_Motor *motorRB = nullptr;
    iarduino_I2C_Motor *motors[4] = {nullptr, nullptr, nullptr, nullptr};

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

    void setMotors(iarduino_I2C_Motor *motorLF, iarduino_I2C_Motor *motorRF, iarduino_I2C_Motor *motorLB, iarduino_I2C_Motor *motorRB);

    void setController(Controller *controller);

    void setMotorSpeed(iarduino_I2C_Motor *motor, int speed);

    void updateSpeed();

    void loop();
};

void robotSetup();

void robotBegin(void *params);

void robotLoop();
