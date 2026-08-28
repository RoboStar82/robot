
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"
#include "print.h"

class Python {
   public:
    Python();
    ~Python();

    void begin();

    void end();

    void task();

   protected:
    TaskHandle_t taskStarted = nullptr;

    static inline void task(void* arg);
};

extern Python python;
