
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#if true
#include "config.h"
#include "delay.h"
#endif

void vTaskDelayMS(uint32_t ms) {
#ifdef ARDUINO_STM32
    if (!xTaskGetCurrentTaskHandle()) {
        return delay(ms);
    }
#endif
    if (ms <= 1) {
        return vTaskDelay(ms);
    }
    vTaskDelay(pdMS_TO_TICKS(ms));
}
