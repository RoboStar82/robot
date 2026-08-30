
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include <string>

#include "config.h"

class Writer : public Print {
   public:
    Writer();
    virtual ~Writer() = default;

    void begin();
    void end();

    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buffer, size_t length) override;
    void flush() override;

    size_t send(Stream& stream, const std::string& output);

    void task();

    explicit operator bool() const {
        return taskHandle != nullptr;
    }

    static int write(void* cookie, const char* buffer, int length);

   protected:
    TaskHandle_t taskHandle = nullptr;

    FILE* stdoutReplaced = nullptr;

    SemaphoreHandle_t txLock = xSemaphoreCreateMutex();

    uint8_t txBuffer[4096];
    size_t txLength = 0;

    static inline void task(void* arg);
};

extern Writer writer;
