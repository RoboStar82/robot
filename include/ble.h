
#pragma once

#include <NimBLEDevice.h>

#include "ble_battery.h"
#include "ble_robot.h"
#include "ble_uart.h"

class BLE : BLEServerCallbacks {
   public:
    BLE();
    ~BLE();

    BLEServer* server = nullptr;
    BLEAdvertising* advertising = nullptr;

    BLEBattery battery;
    BLERobot robot;
    BLEUart uart;

    void begin();
    void startAdvertising();
    void stopAdvertising();
    void end();

    void onConnect(BLEServer* bleServer, BLEConnInfo& connInfo);
    void onDisconnect(BLEServer* bleServer, BLEConnInfo& connInfo, int reason);

   protected:
    bool started = false;
    bool connected = false;
    uint32_t advertisingDuration = 9999;
};

extern BLE ble;
