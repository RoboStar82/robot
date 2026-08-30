
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <string>

#include "config.h"
#include "print.h"

typedef struct {
    const char* source;
    size_t length;
    const char* filename;
    bool* finished;
    bool buffering;
} PythonCode_t;

class Python {
   public:
    Python();
    virtual ~Python() = default;

    void begin();
    void end();

    void run(const char* source, size_t length, std::string& output, const char* filename = nullptr);
    void run(const char* source, size_t length, const char* filename = nullptr, bool wait = true);

    void write(const char* buffer, size_t length);

    void task();

   protected:
    TaskHandle_t taskHandle = nullptr;
    QueueHandle_t taskQueue = xQueueCreate(4, sizeof(PythonCode_t));

    std::string outputBuffer;
    bool outputBuffering = false;

    uint8_t* heap[8192];

    bool initCalled = false;

    void init();

    void exec(const char* source, size_t length, const char* filename, bool* finished, bool buffering);

    static inline void task(void* arg);
};

extern Python python;
