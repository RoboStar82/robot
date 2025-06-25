
#define debugPosition 0
#define debugPositionTx 0

#include "position.h"

BLEUUID robotPositionCharacteristicUuid("b0b0c7ab-0002-4000-8000-000000000000");

BLECharacteristic *robotPositionCharacteristic = nullptr;

// Позиция робота
uint8_t robotPosition[512] = {0, 0};

uint8_t *getRobotPosition() {
    return robotPosition;
}

BLECharacteristic *getRobotPositionCharacteristic() {
    return robotPositionCharacteristic;
}

void robotPositionSetup(BLEService *robotService) {
    BLE2902 *robotPosition2902 = new BLE2902();
    robotPosition2902->setIndications(true);
    robotPosition2902->setNotifications(true);
    BLEDescriptor *robotPosition2901 = new BLEDescriptor((uint16_t)0x2901);
    robotPosition2901->setValue("Robot Position");
    robotPositionCharacteristic = robotService->createCharacteristic(robotPositionCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    robotPositionCharacteristic->addDescriptor(robotPosition2902);
    robotPositionCharacteristic->addDescriptor(robotPosition2901);
}

void robotPositionLoop() {
    Lidar *lidar = getLidar();

    if (!lidar->started) {
        delay(1000);
        return;
    }

    int length = 0;
    lidar->copyLoop(robotPosition + 2, length);

    iarduino_Position_BMX055 *bmx = getBMX();
    int angle = (360 + (int)bmx->axisZ) % 360;
    robotPosition[0] = angle & 0xff;
    length++;
    robotPosition[1] = angle >> 8;
    length++;

#if debugPosition
    debug("V: BMX: angle: %d\n", angle);
    debug("V: lidar: angle: %d\n", robotPosition[2]);
#endif

#if debugPositionTx
    debug("V: BLE: position: %d:", length);
    for (int n = 0; n < length; n++) {
        debug(" %02x", robotPosition[n]);
    }
    println();
#endif

    BLECharacteristic *robotPositionCharacteristic = getRobotPositionCharacteristic();
    robotPositionCharacteristic->setValue(robotPosition, length);
    robotPositionCharacteristic->notify();

    delay(2000);
}
