
#pragma once

#include <Arduino.h>

#include "config.h"

#ifdef ROBOT_HAS_OTA_UART
#include "ota_uart.h"
#define RobotSerial otaUart
#else
#define RobotSerial Serial
#endif

void print(const char* format, ...)
    __attribute__((format(printf, 1, 2)));
