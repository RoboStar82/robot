
#include "ota.h"

#include "ble.h"
#include "led.h"
#include "settings.h"

OTA ota;

OTA::OTA() {}

OTA::~OTA() {}

void OTA::begin() {
    ArduinoOTA.setHostname(NET_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    if (!started) {
        xTaskCreate(task, "ota_task", 4096, NULL, 1, NULL);
        xQueueSend(needQueue, settings.getOtaMode());
        started = true;
    }
}

void OTA::beginBLE() {
    if (otaMode & OTA_WIFI) {
        otaMode = OTA_ALL;
    } else {
        otaMode = OTA_BLE;
    }
    ble.begin();
    led.setOtaBLE(true);
}

void OTA::beginWiFi() {
    if (otaMode & OTA_BLE) {
        otaMode = OTA_ALL;
    } else {
        otaMode = OTA_WIFI;
    }
    wifiMode = settings.getWiFiMode();
    String ssid = settings.getWiFiSSID();
    String password = settings.getWiFiPassword();
    if (wifiMode == WIFI_MODE_STA) {
        log_i("Wi-Fi: STA %s", ssid.c_str());
        WiFi.setHostname(NET_HOSTNAME);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        led.setOtaWiFi(true);
    } else if (wifiMode == WIFI_MODE_AP) {
        log_i("Wi-Fi: AP %s", ssid.c_str());
        WiFi.softAPsetHostname(NET_HOSTNAME);
        WiFi.softAP(ssid, password);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        led.setOtaWiFi(true);
    }
}

void OTA::enableBLE() {
    settings.addOtaBLE();
    beginBLE();
}

void OTA::enableWiFi() {
    settings.addOtaWiFi();
    beginWiFi();
}

void OTA::disableBLE() {
    settings.removeOtaBLE();
    endBLE();
}

void OTA::disableWiFi() {
    settings.removeOtaWiFi();
    endWiFi();
}

void OTA::needEnableBLE() {
    xQueueSend(needQueue, OTA_BLE);
}

void OTA::needEnableWiFi() {
    xQueueSend(needQueue, OTA_WIFI);
}

void OTA::needDisableBLE() {
    xQueueSend(needQueue, -OTA_BLE);
}

void OTA::needDisableWiFi() {
    xQueueSend(needQueue, -OTA_WIFI);
}

void OTA::endBLE() {
    ble.end();
    led.setOtaBLE(false);
    if (otaMode & OTA_WIFI) {
        otaMode = OTA_WIFI;
    } else {
        otaMode = OTA_OFF;
    }
}

void OTA::endWiFi() {
    log_i("Wi-Fi: Disconnect");
    if (wifiMode == WIFI_MODE_STA) {
        WiFi.disconnect();
    } else if (wifiMode == WIFI_MODE_AP) {
        WiFi.softAPdisconnect();
    }
    led.setOtaWiFi(false);
    if (otaMode & OTA_BLE) {
        otaMode = OTA_BLE;
    } else {
        otaMode = OTA_OFF;
    }
}

void OTA::task() {
    if (xQueueReceive(needQueue, &otaMode, 1000)) {
        switch (otaMode) {
            case OTA_OFF:
                vTaskDelay(1000);
                break;
            case OTA_BLE:
                vTaskDelay(1000);
                beginBLE();
                vTaskDelay(1000);
                break;
            case OTA_WIFI:
                vTaskDelay(1000);
                beginWiFi();
                vTaskDelay(1000);
                break;
            case OTA_ALL:
                vTaskDelay(1000);
                beginBLE();
                vTaskDelay(1000);
                beginWiFi();
                vTaskDelay(1000);
                break;
        }
    }
    while (true) {
        int8_t action;
        if (xQueueReceive(needQueue, &action, 1000)) {
            if (!action) {
            } else if (action == OTA_BLE) {
                enableBLE();
                vTaskDelay(1000);
            } else if (action == -OTA_BLE) {
                disableBLE();
                vTaskDelay(1000);
            } else if (action == OTA_WIFI) {
                enableWiFi();
                vTaskDelay(1000);
            } else if (action == -OTA_WIFI) {
                disableWiFi();
                vTaskDelay(1000);
            }
        }
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
            vTaskDelay(1000);
        } else if (wifiMode == WIFI_MODE_AP) {
            ArduinoOTA.handle();
            vTaskDelay(1000);
        } else {
            vTaskDelay(1000);
        }
    }
}

void OTA::task(void* arg) {
    ota.task();
}
