
#include "settings.h"

BLEUUID settingsCharacteristicUuid("b0b0c7ab-0004-4000-8000-000000000000");

BLECharacteristic *settingsCharacteristic = nullptr;

BLECharacteristic *getSettingsCharacteristic() {
    return settingsCharacteristic;
}

void settingsSetup(BLEService *robotService) {
    settingsCharacteristic = robotService->createCharacteristic(settingsCharacteristicUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    settingsCharacteristic->setCallbacks(new SettingsCharacteristicCallbacks);
}
