
#pragma once

#include <ArduinoOTA.h>
#include <WiFi.h>

#include "config.h"

#ifdef ROBOT_HAS_OTA_HTTP
#include "ota_http.h"
#endif

#ifdef ROBOT_HAS_OTA_UART
#include "ota_uart.h"
#endif

#ifdef ROBOT_HAS_OTA_BLACKMAGIC
#include "ota_blackmagic.h"
#endif

class OTA {
   public:
    OTA();
    ~OTA();

    void begin();

    IPAddress getIP();

    void setWiFiMode(WiFiMode_t);

    void task();

    static void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

   protected:
    TaskHandle_t taskStarted = nullptr;
    QueueHandle_t taskQueue = xQueueCreate(4, sizeof(WiFiMode_t));

    WiFiMode_t wifiMode = WIFI_MODE_NULL;
    wl_status_t wifiStatus = WL_NO_SHIELD;
    bool wifiConnected = false;

    void beginWiFi();
    void endWiFi();

    void beginOTA();
    void endOTA();

    static inline void task(void* arg);
};

extern OTA ota;
