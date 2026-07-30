
#include "config.h"

#ifdef ROBOT_HAS_BLE

#include "ble.h"

BLE ble;

BLE::BLE() {}

BLE::~BLE() {}

void BLE::begin() {
    if (!started) {
        started = true;
        BLEDevice::init("");
#ifdef BLE_SECURITY_PASSKEY
        BLEDevice::setSecurityAuth(true, true, true);
        BLEDevice::setSecurityPasskey(BLE_SECURITY_PASSKEY);
        BLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
#endif
        server = BLEDevice::createServer();
        server->setCallbacks(this);
        advertising = BLEDevice::getAdvertising();
        advertising->setName(BLE_DEVICE_NAME);
        advertising->setAdvertisingCompleteCallback(onAdvertisingComplete);
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
        print("[BLE] Advertising start...\n");
        advertising->start(advertisingDuration);
    }
}

void BLE::stopAdvertising() {
    print("[BLE] Advertising stop\n");
    advertising->stop();
}

void BLE::end() {
    print("[BLE] End\n");
    battery.end();
    robot.end();
    uart.end();
    BLEDevice::deinit();
    advertising = nullptr;
    server = nullptr;
    started = false;
}

void BLE::onConnect(BLEServer* bleServer, BLEConnInfo& connInfo) {
    print("[BLE] Connected\n");
    stopAdvertising();
    connected = true;
}

void BLE::onDisconnect(BLEServer* bleServer, BLEConnInfo& connInfo, int reason) {
    print("[BLE] Disconnected\n");
    connected = false;
}

void BLE::onAdvertisingComplete(BLEAdvertising* advertising) {
    print("[BLE] Advertising complete\n");
}

#endif
