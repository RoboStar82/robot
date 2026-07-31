
#pragma once

#include <ArduinoOTA.h>
#include <WiFi.h>

#include "config.h"
#include "print.h"

#ifdef ROBOT_HAS_OTA_HTTP
#include "ota_http.h"
#endif

#ifdef ROBOT_HAS_OTA_UART
#include "ota_uart.h"
#endif

class OTA {
   public:
    OTA();
    ~OTA();

    void begin();

    void setWiFiMode(wifi_mode_t value);

    void task();

    static void task(void* arg);

    static void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

   protected:
    TaskHandle_t taskStarted = nullptr;
    QueueHandle_t taskQueue = xQueueCreate(4, sizeof(wifi_mode_t));

    wifi_mode_t wifiMode = WIFI_MODE_NULL;
    wl_status_t wifiStatus = WL_NO_SHIELD;
    bool wifiConnected = false;

    void beginWiFi();
    void endWiFi();

    void beginOTA();
    void endOTA();
};

extern OTA ota;
