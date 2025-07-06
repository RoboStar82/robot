
#include "position.h"

#if ROBOT_HAS_LIDAR

#define debugPosition 0
#define debugPositionTx 0

BLEUUID positionCharacteristicUuid("b0b0c7ab-0002-4000-8000-000000000000");

BLECharacteristic *positionCharacteristic = nullptr;

// Позиция робота
uint8_t position[512] = {0, 0};

uint8_t *getPosition() {
    return position;
}

BLECharacteristic *getPositionCharacteristic() {
    return positionCharacteristic;
}

void positionSetup(BLEService *robotService) {
    BLE2902 *position2902 = new BLE2902();
    position2902->setIndications(true);
    position2902->setNotifications(true);
    BLEDescriptor *position2901 = new BLEDescriptor((uint16_t)0x2901);
    position2901->setValue("Robot Position");
    positionCharacteristic = robotService->createCharacteristic(positionCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    positionCharacteristic->addDescriptor(position2902);
    positionCharacteristic->addDescriptor(position2901);
    xTaskCreatePinnedToCore(positionBegin, "position", 4096, NULL, 1, NULL, 1);
}

void positionBegin(void *params) {
    while (true) {
        positionLoop();
    }
}

void positionLoop() {
    Lidar *lidar = getLidar();
    if (lidar->started) {
        int length = 0;
        lidar->copyLoop(position + 2, length);

        int angle = 0;
#if ROBOT_HAS_BMX
        iarduino_Position_BMX055 *bmx = getBMX();
        angle = (360 + (int)bmx->axisZ) % 360;
#endif
        position[0] = angle & 0xff;
        length++;
        position[1] = angle >> 8;
        length++;

#if debugPosition
        debug("V: BMX: angle: %d\n", angle);
        debug("V: lidar: angle: %d\n", position[2]);
#endif

#if debugPositionTx
        debug("V: BLE: position: %d:", length);
        for (int n = 0; n < length; n++) {
            debug(" %02x", position[n]);
        }
        println();
#endif

        BLECharacteristic *positionCharacteristic = getPositionCharacteristic();
        positionCharacteristic->setValue(position, length);
        positionCharacteristic->notify();
    }
    delay(3000);
}

#endif
