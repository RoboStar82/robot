
#include <Arduino.h>
#include <FreeRTOS.h>
#include <message_buffer.h>
#include <task.h>

#if true
#include "print.h"
#endif

MessageBufferHandle_t printMessageBuffer = nullptr;
StaticMessageBuffer_t printMessageBufferStruct;
uint8_t printBuffer[4096] = {0};

void taskPrint(void* arg) {
    char buffer[1024];
    size_t length;
    while (true) {
        length = xMessageBufferReceive(printMessageBuffer, buffer, sizeof(buffer), pdMS_TO_TICKS(1000));
        if (length > 0) {
            RobotSerial.write(buffer, length);
        }
    }
}

void print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprint(format, args);
    va_end(args);
}

void print(const uint8_t* buffer, size_t length) {
#ifdef ARDUINO_STM32
    if (!xTaskGetCurrentTaskHandle()) {
        RobotSerial.write(buffer, length);
        return;
    }
#endif
    if (length && printMessageBuffer && !xMessageBufferIsFull(printMessageBuffer)) {
        xMessageBufferSend(printMessageBuffer, buffer, min(length, xMessageBufferSpaceAvailable(printMessageBuffer)), 0);
    }
}

void vprint(const char* format, va_list args) {
#ifdef ARDUINO_STM32
    if (!xTaskGetCurrentTaskHandle()) {
        RobotSerial.vprintf(format, args);
        return;
    }
#endif
    if (!printMessageBuffer) {
        printMessageBuffer = xMessageBufferCreateStatic(sizeof(printBuffer), printBuffer, &printMessageBufferStruct);
        xTaskCreate(taskPrint, "print_task", 4096, NULL, 0, NULL);
    }
    char buffer[1024];
    size_t length = vsnprintf(buffer, sizeof(buffer), format, args);
    print(buffer, length);
}
