
#include "settings.h"

Settings settings;

Settings::Settings() : Preferences() {}

Settings::~Settings() {}

void Settings::begin() {
    begin("robot");
    started = true;
    read();
}

void Settings::read() {
    if (isKey("ota.mode")) {
        otaMode = (ota_mode_t)getUChar("ota.mode");
    }
    if (isKey("wifi.mode")) {
        wifiMode = (wifi_mode_t)getUChar("wifi.mode");
    }
    if (isKey("wifi.ssid")) {
        wifiSSID = getString("wifi.ssid");
    }
    if (isKey("wifi.password")) {
        wifiPassword = getString("wifi.password");
    }
    if (isKey("robot.settings")) {
        robotSettings = getString("robot.settings");
    }
}

ota_mode_t getOtaMode() {
    return otaMode;
}

bool hasOtaBLE() {
    return otaMode & OTA_BLE;
}

bool hasOtaWiFi() {
    return otaMode & OTA_WIFI;
}

bool setOtaMode(ota_mode_t value) {
    otaMode = value;
    return true;
}

bool addOtaBLE() {
    if (otaMode & OTA_WIFI) {
        return setOtaMode(OTA_ALL);
    } else {
        return setOtaMode(OTA_BLE);
    }
}

bool addOtaWiFi() {
    if (otaMode & OTA_BLE) {
        return setOtaMode(OTA_ALL);
    } else {
        return setOtaMode(OTA_WIFI);
    }
}

bool removeOtaBLE() {
    if (otaMode & OTA_WIFI) {
        return setOtaMode(OTA_WIFI);
    } else {
        return setOtaMode(OTA_OFF);
    }
}

bool removeOtaWiFi() {
    if (otaMode & OTA_BLE) {
        return setOtaMode(OTA_BLE);
    } else {
        return setOtaMode(OTA_OFF);
    }
}

bool setWiFiMode(wifi_mode_t value) {
    wifiMode = value;
    putUChar("wifi.mode", value);
    return true;
}

bool setWiFiSSID(String value) {
    wifiSSID = value;
    putString("wifi.ssid", value);
    return true;
}

bool setWiFiPassword(String value) {
    wifiPassword = value;
    putString("wifi.password", value);
    return true;
}

bool setRobotSettings(String value) {
    robotSettings = value;
    putString("robot.settings", value);
    return true;
}

void Settings::end() {
    Preferences::end();
    started = false;
}
