
#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "config.h"
#include "delay.h"

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
