
#pragma once

#include <NimBLEDevice.h>

// client write server read
class BLEUartRx : BLECharacteristicCallbacks {
   public:
    BLEUartRx();
    ~BLEUartRx();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
    const char* characteristicDescription = "UART Rx";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

// server write client read
class BLEUartTx {
   public:
    BLEUartTx();
    ~BLEUartTx();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
    const char* characteristicDescription = "UART Tx";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

class BLEUart : public BLECharacteristicCallbacks {
   public:
    BLEUart();
    ~BLEUart();

    BLEService* service = nullptr;

    // client write server read
    BLEUartRx rx;

    // server write client read
    BLEUartTx tx;

    void begin();
    void end();

   protected:
    BLEUUID serviceUuid = BLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
};
