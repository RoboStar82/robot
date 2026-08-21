
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
#ifdef ARDUINO_STM32
    if (!xTaskGetCurrentTaskHandle()) {
        va_list args;
        va_start(args, format);
        RobotSerial.vprintf(format, args);
        va_end(args);
        return;
    }
#endif
    if (printMessageBuffer == nullptr) {
        printMessageBuffer = xMessageBufferCreateStatic(sizeof(printBuffer), printBuffer, &printMessageBufferStruct);
        xTaskCreate(taskPrint, "print_task", 4096, NULL, 0, NULL);
    }
    char buffer[1024];
    size_t length;
    va_list args;
    va_start(args, format);
    length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    if (!xMessageBufferIsFull(printMessageBuffer) && length > 0) {
        xMessageBufferSend(printMessageBuffer, buffer, min(length, xMessageBufferSpaceAvailable(printMessageBuffer)), 0);
    }
}
