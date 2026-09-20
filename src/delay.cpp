
#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "config.h"
#include "delay.h"

void vTaskDelayMS(uint32_t ms) {
#ifdef ARDUINO_ARCH_STM32
    if (!xTaskGetCurrentTaskHandle()) {
        return delay(ms);
    }
#endif
    if (ms <= 1) {
        return vTaskDelay(ms);
    }
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void vTaskDelayMicroseconds(uint32_t us) {
#ifdef ARDUINO_ARCH_STM32
    if (!xTaskGetCurrentTaskHandle()) {
        return delayMicroseconds(us);
    }
#endif
    uint32_t start = micros();
    while (true) {
        taskYIELD();
        if (micros() - start >= us) {
            return;
        }
    }
}
