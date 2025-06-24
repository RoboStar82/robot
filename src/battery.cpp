
#include "battery.h"

uint8_t batteryLevel = 0;

bool batteryDebug = false;

BLEUUID batteryServiceUuid((uint16_t)0x180f);
BLEService *batteryService;

BLE2902 *batteryLevel2902;
BLEDescriptor *batteryLevel2901;
BLEUUID batteryLevelCharacteristicUuid((uint16_t)0x2a19);
BLECharacteristic *batteryLevelCharacteristic;

BLEService *getBatteryService() {
    return batteryService;
}

BLECharacteristic *getBatteryLevelCharacteristic() {
    return batteryLevelCharacteristic;
}

void batterySetup(BLEServer *bleServer) {
    pinMode(batteryPin, INPUT);
    batteryService = bleServer->createService(batteryServiceUuid);
    batteryLevel2902 = new BLE2902();
    batteryLevel2902->setIndications(true);
    batteryLevel2902->setNotifications(true);
    batteryLevel2901 = new BLEDescriptor((uint16_t)0x2901);
    batteryLevel2901->setValue("Battery Level");
    batteryLevelCharacteristic = batteryService->createCharacteristic(batteryLevelCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    batteryLevelCharacteristic->addDescriptor(batteryLevel2902);
    batteryLevelCharacteristic->addDescriptor(batteryLevel2901);
    batteryService->start();
    BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
    bleAdvertising->addServiceUUID(batteryServiceUuid);
    xTaskCreatePinnedToCore(batteryBegin, "battery", 4096, NULL, 1, NULL, 1);
}

void batteryBegin(void *params) {
    delay(1000);
    while (true) {
        batteryLoop();
    }
}

void batteryLoop() {
    /*
     * 2625 ~ 16.6V
     * 2065 ~ 12.7V
     * (16.6 - 12.7) / (2625 - 2065)
     * (16.6 - 12.7) / (2625 - 2065) * 2625 - 16.6 = 0.00696
     * (16.6 - 12.7) / (2625 - 2065) * 2065 - 12.7 = 1.68
     */
    uint16_t value = analogRead(batteryPin);
    if (batteryDebug) {
        debug("V: battery: value: %d\n", value);
    }
    float voltage = 0.00696f * value - 1.68f;
    if (batteryDebug && voltage > 10.0f) {
        debug("V: battery: voltage: %f\n", voltage);
    }
    int newBatteryLevel = (100.0f * (voltage - 12.0f) / (16.6f - 12.0f));
    if (newBatteryLevel < 0) {
        newBatteryLevel = 0;
    } else if (newBatteryLevel > 100) {
        newBatteryLevel = 100;
    }
    if ((uint8_t)batteryLevel != (uint8_t)newBatteryLevel) {
        batteryLevel = newBatteryLevel;
        debug("V: battery: %d%%\n", batteryLevel);
        batteryLevelCharacteristic->setValue(&batteryLevel, 1);
        batteryLevelCharacteristic->notify();
    }
    delay(9000);
}
