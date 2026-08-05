
#include "config.h"

#ifdef ROBOT_HAS_BLE

#include "ble.h"

BLEBatteryLevel::BLEBatteryLevel() {}

BLEBatteryLevel::~BLEBatteryLevel() {}

void BLEBatteryLevel::begin(BLEService* service) {
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
    if (batteryPin) {
        pinMode(batteryPin, INPUT);
        if (!startedTask) {
            xTaskCreate(task, "battery_level_task", 4096, NULL, 1, &startedTask);
        }
    }
}

void BLEBatteryLevel::end() {
    characteristic = nullptr;
}

void BLEBatteryLevel::task(void* arg) {
    ble.battery.level.task();
}

void BLEBatteryLevel::task() {
    uint32_t voltage = 0;
    int lowCount = 0;
    while (true) {
        uint32_t voltages[4] = {0, 0, 0, 0};
        voltage = analogReadMilliVolts(batteryPin);
        voltages[0] = voltage;
        delay(10);
        voltage = analogReadMilliVolts(batteryPin);
        if (voltage < voltages[0]) {
            voltages[1] = voltages[0];
            voltages[0] = voltage;
        } else {
            voltages[1] = voltage;
        }
        delay(10);
        voltage = analogReadMilliVolts(batteryPin);
        if (voltage < voltages[0]) {
            voltages[2] = voltages[1];
            voltages[1] = voltages[0];
            voltages[0] = voltage;
        } else if (voltage < voltages[1]) {
            voltages[2] = voltages[1];
            voltages[1] = voltage;
        } else {
            voltages[2] = voltage;
        }
        delay(10);
        voltage = analogReadMilliVolts(batteryPin);
        if (voltage < voltages[0]) {
            voltages[3] = voltages[2];
            voltages[2] = voltages[1];
            voltages[1] = voltages[0];
            voltages[0] = voltage;
        } else if (voltage < voltages[1]) {
            voltages[3] = voltages[2];
            voltages[2] = voltages[1];
            voltages[1] = voltage;
        } else if (voltage < voltages[2]) {
            voltages[3] = voltages[2];
            voltages[2] = voltage;
        } else {
            voltages[3] = voltage;
        }
        voltage = (voltages[1] + voltages[2]) >> 1;
        if (voltage > maxVoltage) {
            setValue(100);
            lowCount = 0;
        } else if (voltage > minVoltage) {
            setValue(round(100.0f * (voltage - minVoltage) / (maxVoltage - minVoltage)));
            lowCount = 0;
        } else if (voltage > 0) {
            if (++lowCount > 9) {
                esp_deep_sleep_start();
            }
        }
        delay(10000);
    }
}

void BLEBatteryLevel::setValue(uint8_t _value) {
    if (value == _value) {
        return;
    }
    value = _value;
    log_i("Battery: %d%%", value);
    if (characteristic) {
        characteristic->setValue(value);
        characteristic->indicate();
    }
}

void BLEBattery::begin() {
    service = ble.server->createService(serviceUuid);
    level.begin(service);
}

void BLEBattery::end() {
    level.end();
    service = nullptr;
}

#endif
