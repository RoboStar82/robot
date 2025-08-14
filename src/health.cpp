
#include "health.h"

BLEUUID healthCharacteristicUuid("b0b0c7ab-0003-4000-8000-000000000000");

BLECharacteristic *healthCharacteristic = nullptr;

// Состояние робота
uint8_t health[5] = {
    (BUILD_TIMESTAMP >> 0) & 0xff,
    (BUILD_TIMESTAMP >> 8) & 0xff,
    (BUILD_TIMESTAMP >> 16) & 0xff,
    (BUILD_TIMESTAMP >> 24) & 0xff,
    0};

uint8_t *getHealth() {
    return health;
}

int getHealthSize() {
    return sizeof(health);
}

BLECharacteristic *getHealthCharacteristic() {
    return healthCharacteristic;
}

void healthSetup(BLEService *robotService) {
    healthCharacteristic = robotService->createCharacteristic(healthCharacteristicUuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY);
}
