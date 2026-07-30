
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
    if (!startedTask) {
        xTaskCreate(task, "ota_task", 16384, NULL, 1, &startedTask);
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
    xQueueSend(modeQueue, &value, 0);
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
        print("[Wi-Fi] STA %s\n", ssid.c_str());
        WiFi.setHostname(NET_HOSTNAME);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, password);
        WiFi.setTxPower(WIFI_POWER_20dBm);
#ifdef ROBOT_HAS_LED
        led.setWiFi(true);
#endif
    } else if (wifiMode == WIFI_MODE_AP) {
        print("[Wi-Fi] AP %s\n", ssid.c_str());
        WiFi.softAPsetHostname(NET_HOSTNAME);
        WiFi.softAP(ssid, password);
        WiFi.setTxPower(WIFI_POWER_20dBm);
#ifdef ROBOT_HAS_LED
        led.setWiFi(true);
#endif
        print("[Wi-Fi] Enabled: %s\n", WiFi.softAPIP().toString().c_str());
        ArduinoOTA.begin();
#ifdef ROBOT_HAS_OTA_UART
        otaUart.begin();
#endif
    }
}

void OTA::endWiFi() {
    print("[Wi-Fi] Disconnect\n");
    if (wifiMode == WIFI_MODE_STA) {
        WiFi.setAutoReconnect(false);
        WiFi.disconnect(true);
    } else if (wifiMode == WIFI_MODE_AP) {
        WiFi.softAPdisconnect(true);
    }
#ifdef ROBOT_HAS_LED
    led.setWiFi(false);
#endif
}

void OTA::task() {
    WiFi.onEvent(onWiFiEvent);
    while (true) {
        wifi_mode_t value;
        if (xQueueReceive(modeQueue, &value, 1000)) {
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
                    print("[Wi-Fi] Connected: %s\n", WiFi.localIP().toString().c_str());
                    wifiConnected = true;
                    ArduinoOTA.begin();
#ifdef ROBOT_HAS_OTA_UART
                    otaUart.begin();
#endif
                } else {
                    if (wifiConnected) {
                        print("[Wi-Fi] Disconnected\n");
                        wifiConnected = false;
                        ArduinoOTA.end();
#ifdef ROBOT_HAS_OTA_UART
                        otaUart.end();
#endif
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
    switch (event) {
        case ARDUINO_EVENT_WIFI_OFF:
            print("[Wi-Fi] Event: off\n");
            break;
        case ARDUINO_EVENT_WIFI_READY:
            print("[Wi-Fi] Event: ready\n");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            print("[Wi-Fi] Event: scan done\n");
            break;
        case ARDUINO_EVENT_WIFI_FTM_REPORT:
            print("[Wi-Fi] Event: FTM report\n");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            print("[Wi-Fi] Event: STA start\n");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            print("[Wi-Fi] Event: STA stop\n");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            print("[Wi-Fi] Event: STA connected\n");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            print("[Wi-Fi] Event: STA disconnected\n");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            print("[Wi-Fi] Event: STA auth mode change\n");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            print("[Wi-Fi] Event: STA got IP\n");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            print("[Wi-Fi] Event: STA got IPv6\n");
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            print("[Wi-Fi] Event: STA lost IP\n");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            print("[Wi-Fi] Event: AP start\n");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            print("[Wi-Fi] Event: AP stop\n");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            print("[Wi-Fi] Event: AP STA connected\n");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            print("[Wi-Fi] Event: AP STA disconnected\n");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            print("[Wi-Fi] Event: AP STA got IP\n");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            print("[Wi-Fi] Event: AP probe request\n");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            break;
        default:
            print("[Wi-Fi] Event: %d\n", event);
    }
}

#endif
