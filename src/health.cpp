
#include "health.h"

BLEUUID healthCharacteristicUuid("b0b0c7ab-0003-4000-8000-000000000000");

BLECharacteristic *healthCharacteristic = nullptr;

// Состояние робота
uint8_t health[1] = { 0 };

uint8_t *getHealth() {
    return health;
}

BLECharacteristic *getHealthCharacteristic() {
    return healthCharacteristic;
}

void healthSetup(BLEService *robotService) {
    healthCharacteristic = robotService->createCharacteristic(healthCharacteristicUuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY);
}
