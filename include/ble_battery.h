
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include <NimBLEDevice.h>

#include "config.h"

// server write client read
class BLEBatteryLevel {
   public:
    BLEBatteryLevel();
    virtual ~BLEBatteryLevel() = default;

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void setValue(uint8_t value);

    void end();

    void task();

   protected:
    BLEUUID characteristicUuid = BLEUUID((uint16_t)0x2a19);
    const char* characteristicDescription = "Battery Level";
    uint8_t characteristicFormat = BLE2904::FORMAT_UINT8;

    TaskHandle_t taskHandle = nullptr;
    uint8_t value = 0;
#ifdef ROBOT_HAS_TRANSCEIVER_LORA
    uint8_t batteryPin = 2;
    uint32_t minVoltage = 1500;
    uint32_t maxVoltage = 2000;
#else
    uint8_t batteryPin = 0;
    uint32_t minVoltage = 0;
    uint32_t maxVoltage = 0;
#endif

    static inline void task(void* arg);
};

class BLEBattery {
   public:
    BLEService* service = nullptr;

    // server write client read
    BLEBatteryLevel level;

    void begin();
    void end();

   protected:
    BLEUUID serviceUuid = BLEUUID((uint16_t)0x180f);
};
