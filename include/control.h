
#pragma once

#include <BLE2902.h>
#include <BLEService.h>

#include "print.h"

class RobotControl {
   public:
    // Кнопки

    // Start
    bool start = false;
    bool changeStart = false;
    // Back
    bool back = false;
    bool changeBack = false;

    // A
    bool A = false;
    bool changeA = false;
    // B
    bool B = false;
    bool changeB = false;
    // X
    bool X = false;
    bool changeX = false;
    // Y
    bool Y = false;
    bool changeY = false;

    // D-pad X (right-left)
    int DX = 0;
    // D-pad Y (up-down)
    int DY = 0;

    // Left X (right-left)
    int LX = 0;
    // Left Y (up-down)
    int LY = 0;
    // Right X (right-left)
    int RX = 0;
    // Right Y (up-down)
    int RY = 0;

    bool changeXY = false;

    // Left Z (up-down)
    int LZ = 0;
    // Right Z (up-down)
    int RZ = 0;

    bool changeZ = false;
};

RobotControl *getRobotControl();

BLECharacteristic *getRobotControlCharacteristic();

class RobotControlCharacteristicCallbacks : public BLECharacteristicCallbacks {
   public:
    void onWrite(BLECharacteristic *bleCharacteristic);
};

void robotControlSetup(BLEService *robotService);
