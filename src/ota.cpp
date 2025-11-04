
#include "ota.h"

#include "ble.h"
#include "led.h"
#include "settings.h"

OTA ota;

void OTA::begin() {
    ArduinoOTA.setHostname(NET_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    int ota = settings.getUChar("ota");
    if (ota & OTA_BLE) {
        beginBLE();
    }
    if (ota & OTA_WIFI) {
        beginWiFi();
    }
}

void OTA::beginBLE() {
    ble.begin();
    led.setOtaBle(true);
}

void OTA::beginWiFi() {
    wifiMode = (wifi_mode_t)settings.getUChar("wifi.mode");
    String ssid = settings.getString("wifi.ssid");
    String password = settings.getString("wifi.password");
    if (wifiMode == WIFI_MODE_STA) {
        log_i("Wi-Fi: STA %s", ssid.c_str());
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        led.setOtaWiFi(true);
    } else if (wifiMode == WIFI_MODE_AP) {
        log_i("Wi-Fi: AP %s", ssid.c_str());
        WiFi.softAP(ssid, password);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        led.setOtaWiFi(true);
    }
    if (!taskCreated) {
        xTaskCreate(task, "ota_task", 4096, NULL, 1, NULL);
        taskCreated = true;
    }
}

void OTA::enableBLE() {
    int ota = settings.getUChar("ota");
    settings.putUChar("ota", OTA_BLE | (ota & OTA_WIFI));
    beginBLE();
}

void OTA::enableWiFi() {
    int ota = settings.getUChar("ota");
    settings.putUChar("ota", OTA_WIFI | (ota & OTA_BLE));
    beginWiFi();
}

void OTA::disableBLE() {
    int ota = settings.getUChar("ota");
    settings.putUChar("ota", ota & OTA_WIFI);
    endBLE();
}

void OTA::disableWiFi() {
    int ota = settings.getUChar("ota");
    settings.putUChar("ota", ota & OTA_BLE);
    endWiFi();
}

void OTA::endBLE() {
    ble.end();
    led.setOtaBle(false);
}

void OTA::endWiFi() {
    log_i("Wi-Fi: Disconnect");
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    led.setOtaWiFi(false);
}

void OTA::task(void* arg) {
    ota.task();
}

void OTA::task() {
    while (true) {
        if (wifiMode == WIFI_MODE_STA) {
            if (wifiStatus != WiFi.status()) {
                wifiStatus = WiFi.status();
                if (wifiStatus == WL_CONNECTED) {
                    log_i("Wi-Fi: Connected");
                    wifiConnected = true;
                    ArduinoOTA.begin();
                } else {
                    if (wifiConnected) {
                        log_i("Wi-Fi: Disconnected");
                        wifiConnected = false;
                        ArduinoOTA.end();
                    }
                }
            }
            if (wifiConnected) {
                ArduinoOTA.handle();
            }
            delay(100);
        } else if (wifiMode == WIFI_MODE_AP) {
            ArduinoOTA.handle();
            delay(100);
        } else {
            delay(1000);
        }
    }
}
