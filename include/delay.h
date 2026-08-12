
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "config.h"

#define xQueueSendMS(xQueue, pvItemToQueue, ms) xQueueSend(xQueue, pvItemToQueue, pdMS_TO_TICKS(ms))
#define xQueueReceiveMS(xQueue, pvBuffer, ms) xQueueReceive(xQueue, pvBuffer, pdMS_TO_TICKS(ms))
#define ulTaskNotifyTakeMS(xClearCountOnExit, ms) ulTaskNotifyTake(xClearCountOnExit, pdMS_TO_TICKS(ms))

#ifdef __cplusplus
extern "C"
#endif
    void vTaskDelayMS(uint32_t ms);
