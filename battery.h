
#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "print.h"

#define batteryPin 4

BLEService *getBatteryService();

BLECharacteristic *getBatteryLevelCharacteristic();

void batterySetup(BLEServer *bleServer);

void batteryBegin(void *params);

void batteryLoop();
