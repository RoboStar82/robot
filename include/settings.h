
#pragma once

#ifndef DEBUG_SETTINGS
#define DEBUG_SETTINGS 1
#endif

#include <NimBLEDevice.h>

#include "print.h"

BLECharacteristic *getSettingsCharacteristic();

class SettingsCharacteristicCallbacks : public BLECharacteristicCallbacks {
   public:
    void onWrite(BLECharacteristic *bleCharacteristic, BLEConnInfo &connInfo);
};

void settingsSetup(BLEService *robotService);
