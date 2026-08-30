
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <blackmagic.h>
#ifdef __cplusplus
}
#endif

#include "config.h"

class OTAGdb {
   public:
    OTAGdb();
    virtual ~OTAGdb() = default;

    void begin();
    void end();

    void write(char c, bool flush);
    void flush(bool force);

    void taskServer();

    WiFiServer server = WiFiServer(ROBOT_OTA_GDB_PORT, 1);
    WiFiClient client;

   protected:
    TaskHandle_t taskMainHandle = nullptr;
    TaskHandle_t taskServerHandle = nullptr;
    char txBuffer[4096];
    size_t txLength = 0;

    static void taskMain(void* arg);
    static inline void taskServer(void* arg);
};

extern OTAGdb otaGdb;
