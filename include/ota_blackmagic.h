
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

#include "print.h"

class OTABlackMagic {
   public:
    OTABlackMagic();
    ~OTABlackMagic();

    void begin();
    void end();

    void write(char c, bool flush);
    void flush(bool force);

    void taskMain();
    void taskServer();

    static void taskMain(void* arg);
    static void taskServer(void* arg);

    WiFiServer server = WiFiServer(ROBOT_OTA_BLACKMAGIC_PORT, 1);
    WiFiClient client;

   protected:
    TaskHandle_t taskMainStarted = nullptr;
    TaskHandle_t taskServerStarted = nullptr;
    char txBuffer[4096];
    unsigned int txLength = 0;
};

extern OTABlackMagic otaBlackMagic;
