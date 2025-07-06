
#pragma once

#include <BLE2902.h>
#include <BLEService.h>

#include "print.h"

class Controller {
   public:
    // Кнопки

    // Start
    bool start = false;
    // Back
    bool back = false;

    // A
    bool A = false;
    // B
    bool B = false;
    // X
    bool X = false;
    // Y
    bool Y = false;

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

    // Left Z (up-down)
    int LZ = 0;
    // Right Z (up-down)
    int RZ = 0;

    void onChangeStart();

    void onChangeBack();

    void onChangeA();

    void onChangeB();

    void onChangeX();

    void onChangeY();

    void onChangeZ();

    void onChangeXY();

};

Controller *getController();

BLECharacteristic *getControllerCharacteristic();

class ControllerCharacteristicCallbacks : public BLECharacteristicCallbacks {
   public:
    void onWrite(BLECharacteristic *bleCharacteristic);
};

void controllerSetup(BLEService *robotService);
