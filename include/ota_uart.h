
#pragma once

#include <Arduino.h>
#include <WiFiServer.h>
#include <WiFiClient.h>

#ifdef ARDUINO_STM32
#include <FreeRTOS.h>
#include <task.h>
#endif

#include "config.h"
#include "print.h"

class OTAUart {
   public:
    OTAUart();
    ~OTAUart();

    void begin();

    size_t printf(const char* format, ...);

    void end();

    void task();

    static void task(void* arg);

   protected:
    TaskHandle_t startedTask = nullptr;

    WiFiServer server = WiFiServer(ROBOT_OTA_UART_PORT, 1);
    WiFiClient client;
};

extern OTAUart otaUart;
