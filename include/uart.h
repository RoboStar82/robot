
#pragma once

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include "print.h"

BLEService *getUARTService();

BLECharacteristic *getUARTRxCharacteristic();

BLECharacteristic *getUARTTxCharacteristic();

class UARTRxCharacteristicCallbacks : public BLECharacteristicCallbacks {
   public:
    void onWrite(BLECharacteristic *bleCharacteristic) {
        debug("V: UART: Rx: %s", bleCharacteristic->getValue().c_str());
    }
};

void uartSetup(BLEServer *bleServer);
