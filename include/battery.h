
#pragma once

#if ROBOT_HAS_BATTERY

#ifndef BATTERY_PIN
#define BATTERY_PIN 4
#endif

#ifndef BATTERY_DEBUG
#define BATTERY_DEBUG 0
#endif

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include "print.h"

uint8_t getBatteryLevel();

BLEService *getBatteryService();

BLECharacteristic *getBatteryLevelCharacteristic();

void batterySetup(BLEServer *bleServer);

void batteryBegin(void *params);

void batteryLoop();

#endif
