
#include "config.h"

#ifdef ROBOT_HAS_BLE

#include "ble.h"
#include "print.h"

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
        print("[BLE] advertising start...\n");
        advertising->start(advertisingDuration);
    }
}

void BLE::stopAdvertising() {
    print("[BLE] advertising stop\n");
    advertising->stop();
}

void BLE::end() {
    print("[BLE] end\n");
    battery.end();
    robot.end();
    uart.end();
    BLEDevice::deinit();
    advertising = nullptr;
    server = nullptr;
    started = false;
}

void BLE::onConnect(BLEServer* bleServer, BLEConnInfo& connInfo) {
    print("[BLE] connected\n");
    stopAdvertising();
    connected = true;
}

void BLE::onDisconnect(BLEServer* bleServer, BLEConnInfo& connInfo, int reason) {
    print("[BLE] disconnected\n");
    connected = false;
}

void BLE::onAdvertisingComplete(BLEAdvertising* advertising) {
    print("[BLE] advertising complete\n");
}

#endif
