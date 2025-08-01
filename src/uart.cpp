
#include "uart.h"

BLEUUID uartServiceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

BLEService *uartService = nullptr;

BLEUUID uartRxCharacteristicUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");

BLEUUID uartTxCharacteristicUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

BLECharacteristic *uartRxCharacteristic = nullptr;

BLECharacteristic *uartTxCharacteristic = nullptr;

BLEService *getUARTService() {
    return uartService;
}

BLECharacteristic *getUARTRxCharacteristic() {
    return uartRxCharacteristic;
}

BLECharacteristic *getUARTTxCharacteristic() {
    return uartTxCharacteristic;
}

void uartSetup(BLEServer *bleServer) {
    uartService = bleServer->createService(uartServiceUuid);
    uartRxCharacteristic = uartService->createCharacteristic(uartRxCharacteristicUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    uartRxCharacteristic->setCallbacks(new UARTRxCharacteristicCallbacks);
    uartTxCharacteristic = uartService->createCharacteristic(uartTxCharacteristicUuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY);
    uartService->start();
    BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
    bleAdvertising->addServiceUUID(uartServiceUuid);
}
