
#include "config.h"

#ifdef ROBOT_HAS_SETTINGS

#include "settings.h"

#ifdef ROBOT_HAS_OTA
#include "ota.h"
#endif

#include "print.h"

Settings settings;

Settings::Settings() : Preferences() {}

Settings::~Settings() {}

void Settings::begin() {
    begin("robot");
    started = true;
    read();
}

void Settings::read() {
#ifdef ROBOT_HAS_OTA
    if (isKey("wifi.mode")) {
        wifiMode = (WiFiMode_t)getUChar("wifi.mode");
    }
    if (isKey("wifi.ssid")) {
        wifiSSID = getString("wifi.ssid");
    }
    if (isKey("wifi.password")) {
        wifiPassword = getString("wifi.password");
    }
#endif
    if (isKey("robot.settings")) {
        robotSettings = getString("robot.settings");
    }
}

#ifdef ROBOT_HAS_OTA

WiFiMode_t Settings::getWiFiMode() {
    return wifiMode;
}

bool Settings::setWiFiMode(WiFiMode_t value) {
    wifiMode = value;
    print("[settings] wifi.mode=%d\n", value);
    putUChar("wifi.mode", value);
    return true;
}

#endif

String Settings::getWiFiSSID() {
    return wifiSSID;
}

bool Settings::setWiFiSSID(String value) {
    wifiSSID = value;
    print("[settings] wifi.ssid=%s\n", value.c_str());
    putString("wifi.ssid", value);
    return true;
}

String Settings::getWiFiPassword() {
    return wifiPassword;
}

bool Settings::setWiFiPassword(String value) {
    wifiPassword = value;
    print("[settings] wifi.password=%s\n", value.c_str());
    putString("wifi.password", value);
    return true;
}

String Settings::getRobotSettings() {
    return robotSettings;
}

bool Settings::setRobotSettings(String value) {
    robotSettings = value;
    print("[settings] robot.settings=%02x\n", value.charAt(0));
    putString("robot.settings", value);
    return true;
}

void Settings::end() {
    Preferences::end();
    started = false;
}

#endif
