
#include "health.h"

BLEUUID robotHealthCharacteristicUuid("b0b0c7ab-0003-4000-8000-000000000000");

BLECharacteristic *robotHealthCharacteristic = nullptr;

// Состояние робота
uint8_t robotHealth[1] = { 0 };

uint8_t *getRobotHealth() {
    return robotHealth;
}

BLECharacteristic *getRobotHealthCharacteristic() {
    return robotHealthCharacteristic;
}

void robotHealthSetup(BLEService *robotService) {
    BLE2902 *robotHealth2902 = new BLE2902();
    robotHealth2902->setIndications(true);
    robotHealth2902->setNotifications(true);
    BLEDescriptor *robotHealth2901 = new BLEDescriptor((uint16_t)0x2901);
    robotHealth2901->setValue("Robot Health");
    robotHealthCharacteristic = robotService->createCharacteristic(robotHealthCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    robotHealthCharacteristic->addDescriptor(robotHealth2902);
    robotHealthCharacteristic->addDescriptor(robotHealth2901);
}
