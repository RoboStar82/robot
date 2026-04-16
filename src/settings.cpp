
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
    if (isKey("reset.reason")) {
        resetReason = getString("reset.reason");
    }
    if (isKey("robot.settings")) {
        robotSettings = getString("robot.settings");
    }
}

ota_mode_t Settings::getOtaMode() {
    return otaMode;
}

bool Settings::hasOtaBLE() {
    return otaMode & OTA_BLE;
}

bool Settings::hasOtaWiFi() {
    return otaMode & OTA_WIFI;
}

bool Settings::setOtaMode(ota_mode_t value) {
    otaMode = value;
    log_i("ota.mode: %d", value);
    return true;
}

bool Settings::addOtaBLE() {
    if (otaMode & OTA_WIFI) {
        return setOtaMode(OTA_ALL);
    } else {
        return setOtaMode(OTA_BLE);
    }
}

bool Settings::addOtaWiFi() {
    if (otaMode & OTA_BLE) {
        return setOtaMode(OTA_ALL);
    } else {
        return setOtaMode(OTA_WIFI);
    }
}

bool Settings::removeOtaBLE() {
    if (otaMode & OTA_WIFI) {
        return setOtaMode(OTA_WIFI);
    } else {
        return setOtaMode(OTA_OFF);
    }
}

bool Settings::removeOtaWiFi() {
    if (otaMode & OTA_BLE) {
        return setOtaMode(OTA_BLE);
    } else {
        return setOtaMode(OTA_OFF);
    }
}

wifi_mode_t Settings::getWiFiMode() {
    return wifiMode;
}

bool Settings::setWiFiMode(wifi_mode_t value) {
    wifiMode = value;
    log_i("wifi.mode: %d", value);
    putUChar("wifi.mode", value);
    return true;
}

String Settings::getWiFiSSID() {
    return wifiSSID;
}

bool Settings::setWiFiSSID(String value) {
    wifiSSID = value;
    log_i("wifi.ssid: %s", value.c_str());
    putString("wifi.ssid", value);
    return true;
}

String Settings::getWiFiPassword() {
    return wifiPassword;
}

bool Settings::setWiFiPassword(String value) {
    wifiPassword = value;
    log_i("wifi.password: %s", value.c_str());
    putString("wifi.password", value);
    return true;
}

bool Settings::setResetReason(String value) {
    resetReason = value;
    log_i("reset.reason: %s", value.c_str());
    putString("reset.reason", value);
    return true;
}

bool Settings::addResetReason(String value) {
    setResetReason(resetReason + '\n' + value);
    return true;
}

String Settings::getResetsReasons() {
    return resetReason;
}

String Settings::getRobotSettings() {
    return robotSettings;
}

bool Settings::setRobotSettings(String value) {
    robotSettings = value;
    log_i("robot.settings: %02x", value.charAt(0));
    putString("robot.settings", value);
    return true;
}

void Settings::end() {
    Preferences::end();
    started = false;
}
