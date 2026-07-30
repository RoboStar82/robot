
#pragma once

#include <Arduino.h>
#include <Preferences.h>

#include "config.h"

#ifdef ROBOT_HAS_OTA
#include "ota.h"
#endif

#include "print.h"

class Settings : Preferences {
   public:
    Settings();
    ~Settings();

    void begin();

    void read();

    void end();

#ifdef ROBOT_HAS_OTA
    wifi_mode_t getWiFiMode();
#endif
    String getWiFiSSID();
    String getWiFiPassword();
#ifdef ROBOT_HAS_OTA
    bool setWiFiMode(wifi_mode_t value);
#endif
    bool setWiFiSSID(String value);
    bool setWiFiPassword(String value);

    String getRobotSettings();
    bool setRobotSettings(String value);

   protected:
    bool started = false;

#ifdef ROBOT_HAS_OTA
    wifi_mode_t wifiMode = WIFI_MODE_NULL;
#endif
    String wifiSSID = "";
    String wifiPassword = "";

    String robotSettings = "";

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
};

extern Settings settings;
