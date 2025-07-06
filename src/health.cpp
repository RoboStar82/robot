
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
    BLE2902 *health2902 = new BLE2902();
    health2902->setIndications(true);
    health2902->setNotifications(true);
    BLEDescriptor *health2901 = new BLEDescriptor((uint16_t)0x2901);
    health2901->setValue("Health");
    healthCharacteristic = robotService->createCharacteristic(healthCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    healthCharacteristic->addDescriptor(health2902);
    healthCharacteristic->addDescriptor(health2901);
}
