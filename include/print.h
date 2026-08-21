
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

#ifdef __cplusplus
extern "C" {
#endif

void print(const uint8_t* buffer, size_t length);

void vprint(const char* format, va_list args);

#ifdef __cplusplus
}
#endif
