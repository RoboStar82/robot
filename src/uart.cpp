
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
    BLE2902 *uartRx2902 = new BLE2902();
    BLEDescriptor *uartRx2901 = new BLEDescriptor((uint16_t)0x2901);
    uartRx2901->setValue("UART Rx");
    uartRxCharacteristic = uartService->createCharacteristic(uartRxCharacteristicUuid, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    uartRxCharacteristic->setCallbacks(new UARTRxCharacteristicCallbacks);
    uartRxCharacteristic->addDescriptor(uartRx2902);
    uartRxCharacteristic->addDescriptor(uartRx2901);
    BLE2902 *uartTx2902 = new BLE2902();
    uartTx2902->setIndications(true);
    uartTx2902->setNotifications(true);
    BLEDescriptor *uartTx2901 = new BLEDescriptor((uint16_t)0x2901);
    uartTx2901->setValue("UART Tx");
    uartTxCharacteristic = uartService->createCharacteristic(uartTxCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    uartTxCharacteristic->addDescriptor(uartTx2902);
    uartTxCharacteristic->addDescriptor(uartTx2901);
    uartService->start();
    BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
    bleAdvertising->addServiceUUID(uartServiceUuid);
}
