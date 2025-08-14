
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

void UARTRxCharacteristicCallbacks::onWrite(BLECharacteristic *bleCharacteristic, BLEConnInfo &connInfo) {
    std::string value = bleCharacteristic->getValue();
    const char *packet = value.data();
    debug("V: UART: Rx: %s\n", packet);
    if (otaHandle(packet)) {
        return;
    }
}

void otaNotify(IPAddress addr, int port) {
    uint8_t value[10];
    value[0] = 'O';
    value[1] = 'T';
    value[2] = 'A';
    value[3] = ':';
    value[4] = addr[0];
    value[5] = addr[1];
    value[6] = addr[2];
    value[7] = addr[3];
    value[8] = port & 0xff;
    value[9] = (port >> 8) & 0xff;
    debug("V: UART: OTA: %s:%d\n", addr.toString().c_str(), port);
    uartTxCharacteristic->indicate(value, sizeof(value));
}

void otaError(const char *error) {
    char value[21];
    int length = snprintf(value, sizeof(value), "OTA:E:%s", error);
    debug("E: UART: %s\n", error);
    uartTxCharacteristic->indicate((const uint8_t *)value, length);
}
