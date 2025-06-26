
#include "control.h"

BLEUUID robotControlCharacteristicUuid("b0b0c7ab-0001-4000-8000-000000000000");

BLECharacteristic *robotControlCharacteristic = nullptr;

RobotControl *robotControl = new RobotControl;

RobotControl *getRobotControl() {
    return robotControl;
}

BLECharacteristic *getRobotControlCharacteristic() {
    return robotControlCharacteristic;
}

void robotControlSetup(BLEService *robotService) {
    BLE2902 *robotControl2902 = new BLE2902();
    BLEDescriptor *robotControl2901 = new BLEDescriptor((uint16_t)0x2901);
    robotControl2901->setValue("Robot Control");
    robotControlCharacteristic = robotService->createCharacteristic(robotControlCharacteristicUuid, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    robotControlCharacteristic->setCallbacks(new RobotControlCharacteristicCallbacks);
    robotControlCharacteristic->addDescriptor(robotControl2902);
    robotControlCharacteristic->addDescriptor(robotControl2901);
}

void RobotControlCharacteristicCallbacks::onWrite(BLECharacteristic *bleCharacteristic) {
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
        if (robotControl->start != newStart) {
            robotControl->start = newStart;
            if (!newStart) {
                robotControl->onChangeStart();
            }
        }
        if (robotControl->back != newBack) {
            robotControl->back = newBack;
            if (!newBack) {
                robotControl->onChangeBack();
            }
        }
        if (robotControl->A != newA) {
            robotControl->A = newA;
            if (!newA) {
                robotControl->onChangeA();
            }
        }
        if (robotControl->B != newB) {
            robotControl->B = newB;
            if (!newB) {
                robotControl->onChangeB();
            }
        }
        if (robotControl->X != newX) {
            robotControl->X = newX;
            if (!newX) {
                robotControl->onChangeX();
            }
        }
        if (robotControl->Y != newY) {
            robotControl->Y = newY;
            if (!newY) {
                robotControl->onChangeY();
            }
        }
        bool changeXY = false;
        if (robotControl->LX != newLX) {
            robotControl->LX = newLX;
            changeXY = true;
        }
        if (robotControl->LY != newLY) {
            robotControl->LY = newLY;
            changeXY = true;
        }
        if (robotControl->RX != newRX) {
            robotControl->RX = newRX;
            changeXY = true;
        }
        if (robotControl->RY != newRY) {
            robotControl->RY = newRY;
            changeXY = true;
        }
        if (robotControl->DX != newDX) {
            robotControl->newDX = newDX;
            changeXY = true;
        }
        if (robotControl->DY != newDY) {
            robotControl->newDY = newDY;
            changeXY = true;
        }
        if (changeXY) {
            robotControl->onChangeXY();
        }
        bool changeZ = false;
        if (robotControl->LZ != newLZ) {
            robotControl->LZ = newLZ;
            changeZ = true;
        }
        if (robotControl->RZ != newRZ) {
            robotControl->RZ = newRZ;
            changeZ = true;
        }
        if (changeZ) {
            robotControl->onChangeZ();
        }
    }
}
