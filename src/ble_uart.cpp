
#include <Arduino.h>

#include "config.h"

#ifdef ROBOT_HAS_BLE

#include "ble.h"

BLEUartRx::BLEUartRx() {}

void BLEUartRx::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // client write server read
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#ifdef BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setCallbacks(this);
}

void BLEUartRx::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    log_i("rx: %s", value.c_str());
}

void BLEUartRx::end() {
    characteristic = nullptr;
}

BLEUartTx::BLEUartTx() {}

void BLEUartTx::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY
#ifdef BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
}

void BLEUartTx::end() {
    characteristic = nullptr;
}

BLEUart::BLEUart() {}

void BLEUart::begin() {
    service = ble.server->createService(serviceUuid);
    rx.begin(service);
    tx.begin(service);
}

void BLEUart::end() {
    rx.end();
    tx.end();
    service = nullptr;
}

#endif
