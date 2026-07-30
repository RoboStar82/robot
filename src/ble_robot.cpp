
#include "config.h"

#ifdef ROBOT_HAS_BLE

#include "ble.h"
#include "ota.h"
#include "settings.h"

BLERobotSettings::BLERobotSettings() {}

BLERobotSettings::~BLERobotSettings() {}

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
    characteristic->setValue(settings.getRobotSettings());
    characteristic->setCallbacks(this);
}

void BLERobotSettings::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    settings.setRobotSettings(value.c_str());
}

void BLERobotSettings::end() {
    characteristic = nullptr;
}

BLERobotWiFiMode::BLERobotWiFiMode() {}

BLERobotWiFiMode::~BLERobotWiFiMode() {}

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
    characteristic->setValue(settings.getWiFiMode());
    characteristic->setCallbacks(this);
}

void BLERobotWiFiMode::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    int8_t wifiMode = value.length() > 0 ? value[0] : 0;
    settings.setWiFiMode((wifi_mode_t)wifiMode);
    ota.setWiFiMode((wifi_mode_t)wifiMode);
}

void BLERobotWiFiMode::end() {
    characteristic = nullptr;
}

BLERobotWiFiSSID::BLERobotWiFiSSID() {}

BLERobotWiFiSSID::~BLERobotWiFiSSID() {}

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
    characteristic->setValue(settings.getWiFiSSID());
    characteristic->setCallbacks(this);
}

void BLERobotWiFiSSID::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    settings.setWiFiSSID(value.c_str());
    ota.begin();
}

void BLERobotWiFiSSID::end() {
    characteristic = nullptr;
}

BLERobotWiFiPassword::BLERobotWiFiPassword() {}

BLERobotWiFiPassword::~BLERobotWiFiPassword() {}

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
    characteristic->setValue(settings.getWiFiPassword());
    characteristic->setCallbacks(this);
}

void BLERobotWiFiPassword::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    settings.setWiFiPassword(value.c_str());
    ota.begin();
}

void BLERobotWiFiPassword::end() {
    characteristic = nullptr;
}

BLERobot::BLERobot() {}

BLERobot::~BLERobot() {}

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
