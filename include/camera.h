
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

class Camera {
   public:
    Camera();
    virtual ~Camera() = default;

    void begin();
    void init();
    void task();
    void end();

   protected:
    TaskHandle_t taskHandle = nullptr;

    static inline void task(void* arg);
};

extern Camera camera;
