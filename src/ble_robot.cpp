
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
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getString("settings"));
    characteristic->setCallbacks(this);
}

void BLERobotSettings::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    log_i("settings: %s", value.c_str());
}

void BLERobotSettings::end() {
    characteristic = nullptr;
}

BLERobotOTA::BLERobotOTA() {}

BLERobotOTA::~BLERobotOTA() {}

void BLERobotOTA::begin(BLEService* service) {
    if (characteristic) {
        return;
    }
    characteristic = service->createCharacteristic(
        characteristicUuid,
        // server write client read
        // client write server read
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getUChar("ota"));
    characteristic->setCallbacks(this);
}

void BLERobotOTA::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    int otaValue = value.length() > 0 ? value[0] : 0;
    settings.putUChar("ota", otaValue);
    log_i("ota: %d", otaValue);
    ota.begin();
}

void BLERobotOTA::end() {
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
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getUChar("wifi.mode"));
    characteristic->setCallbacks(this);
}

void BLERobotWiFiMode::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    int wifiModeValue = value.length() > 0 ? value[0] : 0;
    settings.putUChar("wifi.mode", wifiModeValue);
    log_i("wifi.mode: %d", wifiModeValue);
    ota.begin();
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
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getString("wifi.ssid"));
    characteristic->setCallbacks(this);
}

void BLERobotWiFiSSID::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    settings.putString("wifi.ssid", value.c_str());
    log_i("wifi.ssid: %s", value.c_str());
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
#if BLE_SECURITY_PASSKEY
            | NIMBLE_PROPERTY::READ_AUTHEN | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN | NIMBLE_PROPERTY::WRITE_ENC
#endif
    );
    BLEDescriptor* descriptor = characteristic->createDescriptor(NimBLEUUID((uint16_t)0x2901), NIMBLE_PROPERTY::READ);
    descriptor->setValue(characteristicDescription);
    BLE2904* ble2904 = characteristic->create2904();
    ble2904->setFormat(characteristicFormat);
    characteristic->setValue(settings.getString("wifi.password"));
    characteristic->setCallbacks(this);
}

void BLERobotWiFiPassword::onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo) {
    std::string value = bleCharacteristic->getValue();
    settings.putString("wifi.password", value.c_str());
    log_i("wifi.password: %s", value.c_str());
}

void BLERobotWiFiPassword::end() {
    characteristic = nullptr;
}

BLERobot::BLERobot() {}

BLERobot::~BLERobot() {}

void BLERobot::begin() {
    service = ble.server->createService(serviceUuid);
    settings.begin(service);
    ota.begin(service);
    wifiMode.begin(service);
    wifiSSID.begin(service);
    wifiPassword.begin(service);
    service->start();
}

void BLERobot::end() {
    settings.end();
    ota.end();
    wifiMode.end();
    wifiSSID.end();
    wifiPassword.end();
    service = nullptr;
}