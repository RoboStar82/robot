
#include "ble.h"

void BLEBatteryLevel::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    if (pin && !taskCreated) {
        xTaskCreate(task, "battery_level_task", 4096, NULL, 1, NULL);
        taskCreated = true;
    }
}

void BLEBatteryLevel::end() {
    characteristic = nullptr;
}

void BLEBatteryLevel::task(void* arg) {
    ble.battery.level.task();
}

void BLEBatteryLevel::task() {
    pinMode(pin, INPUT);
    while (true) {
        log_i("%d", analogRead(pin));
        delay(9999);
    }
}

void BLEBattery::begin() {
    service = ble.server->createService(serviceUuid);
    level.begin(service);
    service->start();
}

void BLEBattery::end() {
    level.end();
    service = nullptr;
}
