
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
    if (!stdoutReplaced) {
        stdoutReplaced = _GLOBAL_REENT->_stdout;
        _GLOBAL_REENT->_stdout = funopen(NULL, NULL, write, NULL, NULL);
        setvbuf(_GLOBAL_REENT->_stdout, NULL, _IONBF, 0);
    }
    if (!taskHandle) {
        xTaskCreate(task, "writer_task", 8192, NULL, 0, &taskHandle);
    }
}

void Writer::end() {
    if (stdoutReplaced) {
        _GLOBAL_REENT->_stdout = stdoutReplaced;
        stdoutReplaced = nullptr;
    }
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
        if (txSerial <= 4) {
            txSerial = 0;
        } else {
            txSerial --;
        }
    }
    r = 1;
    xSemaphoreGive(txLock);
    if (txLength >= sizeof(txBuffer) || c == '\n' || c == '\r' || c == '\3' || c == '\4' || c == '\0') {
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
        txSerial = 0;
        r = sizeof(txBuffer) - 5;
    } else if (txLength + length > sizeof(txBuffer)) {
        size_t shift = txLength + length - sizeof(txBuffer);
        memmove(txBuffer, txBuffer + shift, sizeof(txBuffer) - length);
        txLength = sizeof(txBuffer);
        txBuffer[0] = txBuffer[4] = '\n';
        txBuffer[1] = txBuffer[2] = txBuffer[3] = '.';
        if (txSerial <= shift + 4) {
            txSerial = 0;
        } else {
            txSerial -= shift + 4;
        }
        r = length;
    } else {
        memcpy(txBuffer + txLength, buffer, length);
        txLength += length;
        r = length;
    }
    xSemaphoreGive(txLock);
    uint8_t c = buffer[txLength - 1];
    if (txLength >= sizeof(txBuffer) || c == '\n' || c == '\r' || c == '\3' || c == '\4' || c == '\0') {
        flush();
    }
    return r;
}

void Writer::flush() {
    if (taskHandle) {
        xTaskNotifyGive(taskHandle);
    }
}

size_t Writer::send(Stream& stream, const uint8_t* buffer, size_t length) {
    size_t index = 0;
    while (index < length) {
        if (size_t size = stream.write(&buffer[index], length - index)) {
            index += size;
        } else {
            break;
        }
    }
    return index;
}

int Writer::write(void* cookie, const char* buffer, int length) {
    return writer.write((const uint8_t*)buffer, length);
}

void Writer::task() {
    while (true) {
        if (ulTaskNotifyTakeMS(pdTRUE, 1000) || txLength > 0) {
            if (!txLength) {
                continue;
            }
            if (txLength > txSerial) {
                if (xSemaphoreTake(txLock, 1) != pdTRUE) {
                    continue;
                }
                size_t length = txLength - txSerial;
                uint8_t buffer[length];
                memcpy(buffer, &txBuffer[txSerial], length);
                txSerial = txLength;
                xSemaphoreGive(txLock);
                send(Serial, buffer, length);
            }
#ifdef ROBOT_HAS_OTA_UART
            if (otaUart.connected()) {
                if (xSemaphoreTake(txLock, 1) != pdTRUE) {
                    continue;
                }
                size_t length = txLength;
                uint8_t buffer[length];
                memcpy(buffer, txBuffer, length);
                txSerial = txLength = 0;
                xSemaphoreGive(txLock);
                send(otaUart, buffer, length);
            }
#endif
            vTaskDelayMS(1);
        }
    }
}

void Writer::task(void* arg) {
    writer.task();
}

#endif
