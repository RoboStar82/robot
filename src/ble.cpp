
#include "ble.h"

BLE ble;

BLE::BLE() {}

BLE::~BLE() {}

void BLE::begin() {
    if (!started) {
        started = true;
        log_i("BLE: Init");
        BLEDevice::init("");
#if BLE_SECURITY_PASSKEY
        BLEDevice::setSecurityAuth(true, true, true);
        BLEDevice::setSecurityPasskey(BLE_SECURITY_PASSKEY);
        BLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
#endif
        server = BLEDevice::createServer();
        server->setCallbacks(this);
        advertising = BLEDevice::getAdvertising();
        advertising->setName(BLE_DEVICE_NAME);
        battery.begin();
        robot.begin();
        uart.begin();
    }
    if (!connected) {
        startAdvertising();
    }
}

void BLE::startAdvertising() {
    if (!ble_gap_adv_active()) {
        log_i("BLE: Advertising start...");
        advertising->start(advertisingDuration);
    }
}

void BLE::stopAdvertising() {
    log_i("BLE: Advertising stop");
    advertising->stop();
}

void BLE::end() {
    log_i("BLE: Deinit");
    battery.end();
    robot.end();
    uart.end();
    BLEDevice::deinit();
    advertising = nullptr;
    server = nullptr;
    started = false;
}

void BLE::onConnect(BLEServer* bleServer, BLEConnInfo& connInfo) {
    log_i("BLE: Connected");
    stopAdvertising();
    connected = true;
}

void BLE::onDisconnect(BLEServer* bleServer, BLEConnInfo& connInfo, int reason) {
    log_i("BLE: Disconnected");
    connected = false;
}
