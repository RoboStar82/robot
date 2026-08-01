
#include "config.h"

#ifdef ROBOT_HAS_OTA

#include "ota.h"

#ifdef ROBOT_HAS_LED
#include "led.h"
#endif

#ifdef ROBOT_HAS_SETTINGS
#include "settings.h"
#endif

OTA ota;

OTA::OTA() {}

OTA::~OTA() {}

void OTA::begin() {
    ArduinoOTA.setHostname(NET_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    if (!taskStarted) {
        xTaskCreate(task, "ota_task", 16384, NULL, 1, &taskStarted);
#ifdef ROBOT_HAS_SETTINGS
        setWiFiMode(settings.getWiFiMode());
#else
#ifdef WIFI_MODE
        setWiFiMode(WIFI_MODE);
#endif
#endif
    } else {
        setWiFiMode(wifiMode);
    }
}

void OTA::setWiFiMode(wifi_mode_t value) {
    xQueueSend(taskQueue, &value, 0);
}

void OTA::beginWiFi() {
#ifdef ROBOT_HAS_SETTINGS
    String ssid = settings.getWiFiSSID();
#else
#ifdef WIFI_SSID
    String ssid = WIFI_SSID;
#else
    String ssid = "";
#endif
#endif
#ifdef ROBOT_HAS_SETTINGS
    String password = settings.getWiFiPassword();
#else
#ifdef WIFI_PASSWORD
    String password = WIFI_PASSWORD;
#else
    String password = "";
#endif
#endif
    if (wifiMode == WIFI_MODE_STA) {
        print("[Wi-Fi] begin STA %s\n", ssid.c_str());
        WiFi.setHostname(NET_HOSTNAME);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
        WiFi.setTxPower(WIFI_POWER_20dBm);
#ifdef ROBOT_HAS_LED
        led.setWiFi(true);
#endif
    } else if (wifiMode == WIFI_MODE_AP) {
        print("[Wi-Fi] begin AP %s\n", ssid.c_str());
        WiFi.softAPsetHostname(NET_HOSTNAME);
        WiFi.softAP(ssid, password);
        WiFi.setTxPower(WIFI_POWER_20dBm);
#ifdef ROBOT_HAS_LED
        led.setWiFi(true);
#endif
        print("[Wi-Fi] enabled: %s\n", WiFi.softAPIP().toString().c_str());
        beginOTA();
    }
}

void OTA::endWiFi() {
    print("[Wi-Fi] end\n");
    if (wifiMode == WIFI_MODE_STA) {
        WiFi.setAutoReconnect(false);
        WiFi.disconnect(true);
    } else if (wifiMode == WIFI_MODE_AP) {
        WiFi.softAPdisconnect(true);
        endOTA();
    }
#ifdef ROBOT_HAS_LED
    led.setWiFi(false);
#endif
}

void OTA::beginOTA() {
    IPAddress ip;
    ArduinoOTA.begin();
    if (wifiMode == WIFI_MODE_STA) {
        ip = WiFi.localIP();
    } else if (wifiMode == WIFI_MODE_AP) {
        ip = WiFi.softAPIP();
    }
    print("[OTA] upload_protocol = espota\n");
    print("[OTA] upload_port = %s\n", ip.toString().c_str());
#ifdef ROBOT_HAS_OTA_HTTP
    otaHttp.begin();
#endif
#ifdef ROBOT_HAS_OTA_UART
    otaUart.begin();
#endif
}

void OTA::endOTA() {
    ArduinoOTA.end();
#ifdef ROBOT_HAS_OTA_HTTP
    otaHttp.end();
#endif
#ifdef ROBOT_HAS_OTA_UART
    otaUart.end();
#endif
}

void OTA::task() {
    WiFi.onEvent(onWiFiEvent);
    while (true) {
        wifi_mode_t value;
        if (xQueueReceive(taskQueue, &value, 1000)) {
            if (wifiMode != WIFI_MODE_NULL) {
                endWiFi();
            }
            wifiMode = value;
            if (wifiMode != WIFI_MODE_NULL) {
                beginWiFi();
            }
        }
        if (wifiMode == WIFI_MODE_STA) {
            if (wifiStatus != WiFi.status()) {
                wifiStatus = WiFi.status();
                if (wifiStatus == WL_CONNECTED) {
                    print("[Wi-Fi] connected: %s\n", WiFi.localIP().toString().c_str());
                    wifiConnected = true;
                    beginOTA();
                } else {
                    if (wifiConnected) {
                        print("[Wi-Fi] disconnected\n");
                        wifiConnected = false;
                        endOTA();
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

void OTA::onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    print("[Wi-Fi] %s\n", NetworkEvents::eventName(event));
    if (event == ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED) {
        print("[Wi-Fi] AP IP: %s\n", IPAddress(info.wifi_ap_staipassigned.ip.addr).toString().c_str());
    }
}

#endif
