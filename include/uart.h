
#pragma once

#include <NimBLEDevice.h>

#include "ota.h"
#include "print.h"

BLEService *getUARTService();

BLECharacteristic *getUARTRxCharacteristic();

BLECharacteristic *getUARTTxCharacteristic();

class UARTRxCharacteristicCallbacks : public BLECharacteristicCallbacks {
   public:
    void onWrite(BLECharacteristic *bleCharacteristic, BLEConnInfo &connInfo);
};

void uartSetup(BLEServer *bleServer);
