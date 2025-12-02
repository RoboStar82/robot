
#pragma once

#include <ArduinoOTA.h>
#include <WiFi.h>

typedef enum {
    OTA_OFF = 0,
    OTA_BLE = 1,
    OTA_WIFI = 2,
    OTA_ALL = 7,
} ota_mode_t;

class OTA {
   public:
    OTA();
    ~OTA();

    void begin();

    void beginBLE();
    void beginWiFi();

    void enableBLE();
    void enableWiFi();

    void disableBLE();
    void disableWiFi();

    void needEnableBLE();
    void needEnableWiFi();

    void needDisableBLE();
    void needDisableWiFi();

    void endBLE();
    void endWiFi();

    void task();

    static void task(void* arg);

   protected:
    bool started = false;
    QueueHandle_t needQueue = xQueueCreate(4, sizeof(int8_t));

    ota_mode_t otaMode = OTA_OFF;

    wifi_mode_t wifiMode = WIFI_MODE_NULL;
    wl_status_t wifiStatus = WL_NO_SHIELD;
    bool wifiConnected = false;
};

extern OTA ota;
