
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <semphr.h>
#include <task.h>

#include "config.h"

class OTAUart : public Stream {
   public:
    OTAUart();

    void begin();
    void begin(unsigned long baud);

    void end();

    int available() override;
    int peek() override;
    int read() override;
    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buffer, size_t length) override;
    void flush() override;

    void task();

    static void task(void* arg);

    static int write(void* cookie, const char* buffer, int length);

    using Print::print;
    using Print::printf;
    using Print::println;
    using Print::vprintf;
    using Print::write;

   protected:
    TaskHandle_t taskStarted = nullptr;

    SemaphoreHandle_t writeLock = xSemaphoreCreateMutex();
    TickType_t waitLock = 100;

    FILE* stdoutReplaced = nullptr;

    WiFiServer server = WiFiServer(ROBOT_OTA_UART_PORT, 1);
    WiFiClient client;

    char txBuffer[4096];
    size_t txLength = 0;
};

extern OTAUart otaUart;
