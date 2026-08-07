
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <blackmagic.h>
#ifdef __cplusplus
}
#endif

#include "config.h"

class OTABlackMagic {
   public:
    OTABlackMagic();
    ~OTABlackMagic();

    void begin();
    void end();

    void write(char c, bool flush);
    void flush(bool force);

    void taskServer();

    WiFiServer server = WiFiServer(ROBOT_OTA_BLACKMAGIC_PORT, 1);
    WiFiClient client;

   protected:
    TaskHandle_t taskMainStarted = nullptr;
    TaskHandle_t taskServerStarted = nullptr;
    char txBuffer[4096];
    size_t txLength = 0;

    static void taskMain(void* arg);
    static inline void taskServer(void* arg);
};

extern OTABlackMagic otaBlackMagic;
