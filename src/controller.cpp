
#include "controller.h"

BLEUUID controllerCharacteristicUuid("b0b0c7ab-0001-4000-8000-000000000000");

BLECharacteristic *controllerCharacteristic = nullptr;

Controller *controller = new Controller;

Controller *getController() {
    return controller;
}

BLECharacteristic *getControllerCharacteristic() {
    return controllerCharacteristic;
}

void controllerSetup(BLEService *robotService) {
    controllerCharacteristic = robotService->createCharacteristic(controllerCharacteristicUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    controllerCharacteristic->setCallbacks(new ControllerCharacteristicCallbacks);
}

void ControllerCharacteristicCallbacks::onWrite(BLECharacteristic *bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    if (value.length() >= 4) {
        uint8_t btnValue = value[0];
        uint8_t padValue = value[1];
        uint8_t leftValue = value[2];
        uint8_t rightValue = value[3];
        bool newStart = (btnValue >> 0) & 1;
        bool newBack = (btnValue >> 1) & 1;
        bool newA = (btnValue >> 2) & 1;
        bool newB = (btnValue >> 3) & 1;
        bool newX = (btnValue >> 4) & 1;
        bool newY = (btnValue >> 5) & 1;
        bool newLT = (btnValue >> 6) & 1;
        bool newRT = (btnValue >> 7) & 1;
        int newDX = ((padValue >> 0) & 1) ? 1 : (((padValue >> 1) & 1) ? -1 : 0);
        int newDY = ((padValue >> 2) & 1) ? 1 : (((padValue >> 3) & 1) ? -1 : 0);
        int newLZ = ((padValue >> 4) & 1) ? 1 : (((padValue >> 5) & 1) ? -1 : 0);
        int newRZ = ((padValue >> 6) & 1) ? 1 : (((padValue >> 7) & 1) ? -1 : 0);
        uint8_t tmpValue;
        tmpValue = (leftValue >> 0) & 0xf;
        int newLX = tmpValue < 8 ? tmpValue : (8 - tmpValue);
        tmpValue = (leftValue >> 4) & 0xf;
        int newLY = tmpValue < 8 ? tmpValue : (8 - tmpValue);
        tmpValue = (rightValue >> 0) & 0xf;
        int newRX = tmpValue < 8 ? tmpValue : (8 - tmpValue);
        tmpValue = (rightValue >> 4) & 0xf;
        int newRY = tmpValue < 8 ? tmpValue : (8 - tmpValue);
        if (controller->start != newStart) {
            controller->start = newStart;
            if (!newStart) {
                controller->onChangeStart();
            }
        }
        if (controller->back != newBack) {
            controller->back = newBack;
            if (!newBack) {
                controller->onChangeBack();
            }
        }
        if (controller->A != newA) {
            controller->A = newA;
            if (!newA) {
                controller->onChangeA();
            }
        }
        if (controller->B != newB) {
            controller->B = newB;
            if (!newB) {
                controller->onChangeB();
            }
        }
        if (controller->X != newX) {
            controller->X = newX;
            if (!newX) {
                controller->onChangeX();
            }
        }
        if (controller->Y != newY) {
            controller->Y = newY;
            if (!newY) {
                controller->onChangeY();
            }
        }
        bool changeXY = false;
        if (controller->LX != newLX) {
            controller->LX = newLX * 16;
            changeXY = true;
        }
        if (controller->LY != newLY) {
            controller->LY = newLY * 16;
            changeXY = true;
        }
        if (controller->LT != newLT) {
            controller->LT = newLT;
            changeXY = true;
        }
        if (controller->RX != newRX) {
            controller->RX = newRX * 16;
            changeXY = true;
        }
        if (controller->RY != newRY) {
            controller->RY = newRY * 16;
            changeXY = true;
        }
        if (controller->RT != newRT) {
            controller->RT = newRT;
            changeXY = true;
        }
        if (controller->DX != newDX) {
            controller->DX = newDX * 64;
            changeXY = true;
        }
        if (controller->DY != newDY) {
            controller->DY = newDY * 64;
            changeXY = true;
        }
        if (changeXY) {
            controller->onChangeXY();
        }
        bool changeZ = false;
        if (controller->LZ != newLZ) {
            controller->LZ = newLZ;
            changeZ = true;
        }
        if (controller->RZ != newRZ) {
            controller->RZ = newRZ;
            changeZ = true;
        }
        if (changeZ) {
            controller->onChangeZ();
        }
    }
}
