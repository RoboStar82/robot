
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

class Camera {
   public:
    Camera();
    ~Camera();

    void begin();
    void task();
    void end();

   protected:
    TaskHandle_t taskStarted = nullptr;

    static inline void task(void* arg);
};

extern Camera camera;
