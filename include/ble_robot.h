
#pragma once

#include <NimBLEDevice.h>

// server write client read
// client write server read
class BLERobotSettings : BLECharacteristicCallbacks {
   public:
    BLERobotSettings();
    ~BLERobotSettings();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0004-4000-8000-000000000000");
    const char* characteristicDescription = "Settings";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

// server write client read
// client write server read
class BLERobotOTA : BLECharacteristicCallbacks {
   public:
    BLERobotOTA();
    ~BLERobotOTA();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0009-4000-8000-000000000000");
    const char* characteristicDescription = "OTA";
    uint8_t characteristicFormat = BLE2904::FORMAT_UINT8;
};

// server write client read
// client write server read
class BLERobotWiFiMode : BLECharacteristicCallbacks {
   public:
    BLERobotWiFiMode();
    ~BLERobotWiFiMode();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0010-4000-8000-000000000000");
    const char* characteristicDescription = "Wi-Fi Mode";
    uint8_t characteristicFormat = BLE2904::FORMAT_UINT8;
};

// server write client read
// client write server read
class BLERobotWiFiSSID : BLECharacteristicCallbacks {
   public:
    BLERobotWiFiSSID();
    ~BLERobotWiFiSSID();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0011-4000-8000-000000000000");
    const char* characteristicDescription = "Wi-Fi SSID";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

// server write client read
// client write server read
class BLERobotWiFiPassword : BLECharacteristicCallbacks {
   public:
    BLERobotWiFiPassword();
    ~BLERobotWiFiPassword();

    BLECharacteristic* characteristic = nullptr;

    void begin(BLEService* service);

    void onWrite(BLECharacteristic* bleCharacteristic, BLEConnInfo& connInfo);

    void end();

   protected:
    BLEUUID characteristicUuid = BLEUUID("b0b0c7ab-0012-4000-8000-000000000000");
    const char* characteristicDescription = "Wi-Fi Password";
    uint8_t characteristicFormat = BLE2904::FORMAT_UTF8;
};

class BLERobot {
   public:
    BLERobot();
    ~BLERobot();

    BLEService* service = nullptr;

    // server write client read
    // client write server read
    BLERobotSettings settings;
    // server write client read
    // client write server read
    BLERobotOTA ota;
    // server write client read
    // client write server read
    BLERobotWiFiMode wifiMode;
    // server write client read
    // client write server read
    BLERobotWiFiSSID wifiSSID;
    // server write client read
    // client write server read
    BLERobotWiFiPassword wifiPassword;

    void begin();
    void end();

   protected:
    BLEUUID serviceUuid = BLEUUID("b0b0c7ab-0000-4000-8000-000000000000");
};
