
#include <Arduino.h>

#include "config.h"

#ifdef ROBOT_HAS_BLE

#include "ble.h"

#ifdef ROBOT_HAS_OTA
#include "ota.h"
#endif

#ifdef ROBOT_HAS_SETTINGS
#include "settings.h"
#endif

BLERobotSettings::BLERobotSettings() {}

void BLERobotSettings::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#ifdef BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
#ifdef ROBOT_HAS_SETTINGS
    characteristic->setValue(settings.getRobotSettings());
#endif
    characteristic->setCallbacks(this);
}

void BLERobotSettings::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
#ifdef ROBOT_HAS_SETTINGS
    settings.setRobotSettings(value.c_str());
#endif
}

void BLERobotSettings::end() {
    characteristic = nullptr;
}

BLERobotWiFiMode::BLERobotWiFiMode() {}

void BLERobotWiFiMode::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#ifdef BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
#ifdef ROBOT_HAS_SETTINGS
#ifdef ROBOT_HAS_OTA
    characteristic->setValue(settings.getWiFiMode());
#endif
#endif
    characteristic->setCallbacks(this);
}

void BLERobotWiFiMode::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    int8_t wifiMode = value.length() > 0 ? value[0] : 0;
#ifdef ROBOT_HAS_SETTINGS
#ifdef ROBOT_HAS_OTA
    settings.setWiFiMode((WiFiMode_t)wifiMode);
#endif
#endif
#ifdef ROBOT_HAS_OTA
    ota.setWiFiMode((WiFiMode_t)wifiMode);
#endif
}

void BLERobotWiFiMode::end() {
    characteristic = nullptr;
}

BLERobotWiFiSSID::BLERobotWiFiSSID() {}

void BLERobotWiFiSSID::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#ifdef BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
#ifdef ROBOT_HAS_SETTINGS
    characteristic->setValue(settings.getWiFiSSID());
#endif
    characteristic->setCallbacks(this);
}

void BLERobotWiFiSSID::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
#ifdef ROBOT_HAS_SETTINGS
    settings.setWiFiSSID(value.c_str());
#endif
#ifdef ROBOT_HAS_OTA
    ota.begin();
#endif
}

void BLERobotWiFiSSID::end() {
    characteristic = nullptr;
}

BLERobotWiFiPassword::BLERobotWiFiPassword() {}

void BLERobotWiFiPassword::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#ifdef BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
#ifdef ROBOT_HAS_SETTINGS
    characteristic->setValue(settings.getWiFiPassword());
#endif
    characteristic->setCallbacks(this);
}

void BLERobotWiFiPassword::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
#ifdef ROBOT_HAS_SETTINGS
    settings.setWiFiPassword(value.c_str());
#endif
#ifdef ROBOT_HAS_OTA
    ota.begin();
#endif
}

void BLERobotWiFiPassword::end() {
    characteristic = nullptr;
}

BLERobot::BLERobot() {}

void BLERobot::begin() {
    service = ble.server->createService(serviceUuid);
    settings.begin(service);
    wifiMode.begin(service);
    wifiSSID.begin(service);
    wifiPassword.begin(service);
}

void BLERobot::end() {
    settings.end();
    wifiMode.end();
    wifiSSID.end();
    wifiPassword.end();
    service = nullptr;
}

#endif
