
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <HardwareTimer.h>
#include <Wire.h>
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

    HardwareTimer timer = HardwareTimer(TIM1);
    uint32_t timerChannel;

    TwoWire wire;

    void setRegister(uint8_t addr, uint8_t value);
    uint8_t getRegister(uint8_t addr);

    uint16_t vendorId;
    uint16_t deviceId;

    static inline void task(void* arg);
};

extern Camera camera;
