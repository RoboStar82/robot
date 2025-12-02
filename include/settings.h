
#pragma once

#include <Arduino.h>
#include <Preferences.h>

#include "ota.h"

class Settings : Preferences {
   public:
    Settings();
    ~Settings();

    using Preferences::begin;

    using Preferences::isKey;

    using Preferences::getBool;
    using Preferences::getBytes;
    using Preferences::getBytesLength;
    using Preferences::getChar;
    using Preferences::getDouble;
    using Preferences::getFloat;
    using Preferences::getInt;
    using Preferences::getLong;
    using Preferences::getLong64;
    using Preferences::getShort;
    using Preferences::getString;
    using Preferences::getType;
    using Preferences::getUChar;
    using Preferences::getUInt;
    using Preferences::getULong;
    using Preferences::getULong64;
    using Preferences::getUShort;

    using Preferences::putBool;
    using Preferences::putBytes;
    using Preferences::putChar;
    using Preferences::putDouble;
    using Preferences::putFloat;
    using Preferences::putInt;
    using Preferences::putLong;
    using Preferences::putLong64;
    using Preferences::putShort;
    using Preferences::putString;
    using Preferences::putUChar;
    using Preferences::putUInt;
    using Preferences::putULong;
    using Preferences::putULong64;
    using Preferences::putUShort;

    void begin();

    void read();

    void end();

    ota_mode_t getOtaMode();
    bool hasOtaBLE();
    bool hasOtaWiFi();
    bool setOtaMode(ota_mode_t value);
    bool addOtaBLE();
    bool addOtaWiFi();
    bool removeOtaBLE();
    bool removeOtaWiFi();

    wifi_mode_t getWiFiMode();
    String getWiFiSSID();
    String getWiFiPassword();
    bool setWiFiMode(wifi_mode_t value);
    void setWiFiSSID(String value);
    void setWiFiPassword(String value);

    String getRobotSettings(String value);

   protected:
    bool started = false;

    ota_mode_t otaMode = OTA_OFF;

    wifi_mode_t wifiMode = WIFI_MODE_NULL;
    String wifiSSID = "";
    String wifiPassword = "";

    String robotSettings = "";
};

extern Settings settings;
