
#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_WRITER

#ifdef ROBOT_HAS_OTA_UART
#include "ota_uart.h"
#endif

#include "delay.h"
#include "writer.h"

Writer writer;

Writer::Writer() : Print() {}

void Writer::begin() {
    if (!taskHandle) {
        stdoutReplaced = _GLOBAL_REENT->_stdout;
        _GLOBAL_REENT->_stdout = funopen(NULL, NULL, write, NULL, NULL);
        setvbuf(_GLOBAL_REENT->_stdout, NULL, _IONBF, 0);
        xTaskCreate(task, "writer_task", 4096, NULL, 0, &taskHandle);
    }
}

void Writer::end() {
    if (taskHandle) {
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
    }
}

size_t Writer::write(uint8_t c) {
    size_t r = 0;
    if (xSemaphoreTake(txLock, 1) != pdTRUE) {
        return r;
    }
    if (txLength < sizeof(txBuffer)) {
        txBuffer[txLength] = c;
        txLength++;
    } else {
        memmove(txBuffer, txBuffer + 1, sizeof(txBuffer) - 1);
        txBuffer[txLength - 1] = c;
        txBuffer[0] = txBuffer[4] = '\n';
        txBuffer[1] = txBuffer[2] = txBuffer[3] = '.';
    }
    r = 1;
    xSemaphoreGive(txLock);
    if (txLength >= sizeof(txBuffer) || c == '\n' || c == '\r' || c == '\3' || c == '\4') {
        flush();
    }
    return r;
}

size_t Writer::write(const uint8_t* buffer, size_t length) {
    size_t r = 0;
    if (!length) {
        return r;
    }
    if (xSemaphoreTake(txLock, 1) != pdTRUE) {
        return r;
    }
    if (length >= sizeof(txBuffer)) {
        memcpy(txBuffer, buffer + length - sizeof(txBuffer), sizeof(txBuffer));
        txLength = sizeof(txBuffer);
        txBuffer[0] = txBuffer[4] = '\n';
        txBuffer[1] = txBuffer[2] = txBuffer[3] = '.';
        r = sizeof(txBuffer) - 5;
    } else if (txLength + length > sizeof(txBuffer)) {
        memmove(txBuffer, txBuffer + txLength - (sizeof(txBuffer) - length), sizeof(txBuffer) - length);
        txLength = sizeof(txBuffer);
        txBuffer[0] = txBuffer[4] = '\n';
        txBuffer[1] = txBuffer[2] = txBuffer[3] = '.';
        r = length;
    } else {
        memcpy(txBuffer + txLength, buffer, length);
        txLength += length;
        r = length;
    }
    xSemaphoreGive(txLock);
    if (txLength >= sizeof(txBuffer) || buffer[txLength - 1] == '\n' || buffer[txLength - 1] == '\r' || buffer[txLength - 1] == '\3' || buffer[txLength - 1] == '\4') {
        flush();
    }
    return r;
}

void Writer::flush() {
    if (taskHandle) {
        xTaskNotifyGive(taskHandle);
    }
}

size_t Writer::send(Stream& stream, const std::string& output) {
    size_t length = output.length();
    size_t index = 0;
    while (index < length) {
        if (size_t size = Serial.write(&output[index], length - index)) {
            index += size;
        } else {
            break;
        }
    }
    return index;
}

int Writer::write(void* cookie, const char* buffer, int length) {
    writer.write((const uint8_t*)buffer, length);
}

void Writer::task() {
    while (true) {
        if (ulTaskNotifyTakeMS(pdTRUE, 1000) || txLength > 0) {
            if (!txLength) {
                continue;
            }
            if (xSemaphoreTake(txLock, 1) != pdTRUE) {
                continue;
            }
            size_t length = txLength;
            uint8_t buffer[length];
            memcpy(buffer, txBuffer, length);
            xSemaphoreGive(txLock);
            std::string output((char*)buffer, length);
            send(Serial, output);
#ifdef ROBOT_HAS_OTA_UART
            send(otaUart, output);
#endif
            vTaskDelayMS(1);
        }
    }
}

void Writer::task(void* arg) {
    writer.task();
}

#endif
