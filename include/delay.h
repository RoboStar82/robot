
#pragma once

#include <Arduino.h>

#ifdef ARDUINO_STM32
#include <FreeRTOS.h>
#include <task.h>
#endif

#include "config.h"

extern void delay(uint32_t ms);
