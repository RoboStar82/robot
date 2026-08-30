
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include <string>

#include "config.h"

class Reader {
   public:
    Reader();
    virtual ~Reader() = default;

    void begin();
    void end();

    size_t read(Stream& stream, std::string& source);

    void task();

   protected:
    TaskHandle_t taskHandle = nullptr;

    static inline void task(void* arg);
};

extern Reader reader;
