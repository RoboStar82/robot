
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <NimBLEDevice.h>

#include "ble_battery.h"
#include "ble_robot.h"
#include "ble_uart.h"
#include "config.h"

class BLE : public BLEServerCallbacks, public BLEClientCallbacks, public BLEAdvertisedDeviceCallbacks {
   public:
    BLE();
    virtual ~BLE() = default;

    BLEServer* server = nullptr;
    BLEAdvertising* advertising = nullptr;

    BLEBattery battery;
    BLERobot robot;
    BLEUart uart;

    void begin();

    void advertisingStart();
    void advertisingStop();

    void end();

    void onConnect(BLEServer* bleServer, BLEConnInfo& info);
    void onDisconnect(BLEServer* bleServer, BLEConnInfo& info, int reason);

    void onConnect(BLEClient* client);
    void onConnectFail(BLEClient* client, int reason);
    void onDisconnect(BLEClient* client, int reason);

    void onDiscovered(const BLEAdvertisedDevice* advertised);
    void onScanEnd(const BLEScanResults& results, int reason);

    bool onConnParamsUpdateRequest(BLEClient* client, const ble_gap_upd_params* params);

    void setControllerAddress(String address);

    void taskController();

    static void onAdvertisingComplete(BLEAdvertising* advertising);

    void onInputNotify(uint8_t* data, size_t length);

    static inline void onReportNotify(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t length, bool notify);

   protected:
    bool started = false;
    bool hasClient = false;
    bool hasController = false;
    TaskHandle_t taskControllerHandle = nullptr;
    QueueHandle_t advertisedQueue = xQueueCreate(4, sizeof(BLEAdvertisedDevice*));
    String controllerAddress = "";
    uint16_t controllerVendorId = 0x00;
    uint16_t controllerProductId = 0x00;

    BLEClient* client = nullptr;
    BLEScan* scan = nullptr;

    void controllerStart();
    void controllerStop();

    void scanStart();
    void scanPause();
    void scanResume();
    void scanStop();

    bool controllerConnect(const BLEAdvertisedDevice* advertised);
    bool controllerConnect(const BLEAddress address);

    BLEUUID hidServiceUuid = BLEUUID((uint16_t)0x1812);
    BLEUUID hidReportDataUuid = BLEUUID((uint16_t)0x2a4d);
    BLEUUID infoServiceUuid = BLEUUID((uint16_t)0x180a);
    BLEUUID infoPnPIdUuid = BLEUUID((uint16_t)0x2a50);

    static inline void taskController(void* arg);
};

extern BLE ble;
