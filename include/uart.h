
#pragma once

#include <NimBLEDevice.h>

#include "print.h"

BLEService *getUARTService();

BLECharacteristic *getUARTRxCharacteristic();

BLECharacteristic *getUARTTxCharacteristic();

class UARTRxCharacteristicCallbacks : public BLECharacteristicCallbacks {
   public:
    void onWrite(BLECharacteristic *bleCharacteristic, BLEConnInfo& connInfo) {
        debug("V: UART: Rx: %s", bleCharacteristic->getValue().c_str());
    }
};

void uartSetup(BLEServer *bleServer);
