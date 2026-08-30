
#pragma once

#include <ArduinoOTA.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <WiFi.h>

#include "config.h"

#ifdef ROBOT_HAS_OTA_GDB
#include "ota_gdb.h"
#endif

#ifdef ROBOT_HAS_OTA_HTTP
#include "ota_http.h"
#endif

#ifdef ROBOT_HAS_OTA_UART
#include "ota_uart.h"
#endif

class OTA {
   public:
    OTA();
    virtual ~OTA() = default;

    void begin();
    void end();

    IPAddress getIP();

    void setWiFiMode(WiFiMode_t);

    void task();

    static void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

   protected:
    TaskHandle_t taskHandle = nullptr;
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
