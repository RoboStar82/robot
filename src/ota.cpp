
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
    if (!startedTask) {
        xTaskCreate(task, "ota_task", 16384, NULL, 1, &startedTask);
        int8_t value = settings.getOtaMode();
        xQueueSend(needQueue, &value, 0);
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
        WiFi.setTxPower(WIFI_POWER_20dBm);
        led.setOtaWiFi(true);
    } else if (wifiMode == WIFI_MODE_AP) {
        log_i("Wi-Fi: AP %s", ssid.c_str());
        WiFi.softAPsetHostname(NET_HOSTNAME);
        WiFi.softAP(ssid, password);
        WiFi.setTxPower(WIFI_POWER_20dBm);
        led.setOtaWiFi(true);
        log_i("Wi-Fi: Enabled: %s", WiFi.softAPIP().toString().c_str());
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
    int8_t value = OTA_BLE;
    xQueueSend(needQueue, &value, 0);
}

void OTA::needEnableWiFi() {
    int8_t value = OTA_WIFI;
    xQueueSend(needQueue, &value, 0);
}

void OTA::needDisableBLE() {
    int8_t value = -OTA_BLE;
    xQueueSend(needQueue, &value, 0);
}

void OTA::needDisableWiFi() {
    int8_t value = -OTA_WIFI;
    xQueueSend(needQueue, &value, 0);
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
        WiFi.setAutoReconnect(false);
        WiFi.disconnect(true);
    } else if (wifiMode == WIFI_MODE_AP) {
        WiFi.softAPdisconnect(true);
    }
    wifiMode = WIFI_MODE_NULL;
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
            if (ble.robot.otaMode.characteristic) {
                ble.robot.otaMode.characteristic->setValue(otaMode);
            }
        }
        if (wifiMode == WIFI_MODE_STA) {
            if (wifiStatus != WiFi.status()) {
                wifiStatus = WiFi.status();
                if (wifiStatus == WL_CONNECTED) {
                    log_i("Wi-Fi: Connected: %s", WiFi.localIP().toString().c_str());
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
