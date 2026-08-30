
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#include "config.h"

class OTAUart : public Stream {
   public:
    OTAUart();
    virtual ~OTAUart() = default;

    void begin();
    void end();

    int available() override;
    int peek() override;
    int read() override;
    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buffer, size_t length) override;
    void flush() override;

    void task();

    using Print::print;
    using Print::printf;
    using Print::println;
    using Print::vprintf;
    using Print::write;

   protected:
    TaskHandle_t taskHandle = nullptr;

    WiFiServer server = WiFiServer(ROBOT_OTA_UART_PORT, 1);
    WiFiClient client;

    static inline void task(void* arg);
};

extern OTAUart otaUart;
