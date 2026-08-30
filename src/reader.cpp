
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include <string>

#include "config.h"

#ifdef ROBOT_HAS_READER

#ifdef ROBOT_HAS_OTA_UART
#include "ota_uart.h"
#endif

#ifdef ROBOT_HAS_PYTHON
#include "python.h"
#endif

#ifdef ROBOT_HAS_SCRIPT
#include "script.h"
#endif

#include "delay.h"
#include "reader.h"

Reader reader;

Reader::Reader() {}

void Reader::begin() {
    if (!taskHandle) {
        xTaskCreate(task, "reader_task", 4096, NULL, 0, &taskHandle);
    }
}

void Reader::end() {
    if (taskHandle) {
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
    }
}

size_t Reader::read(Stream& stream, std::string& source) {
    size_t available = stream.available();
    if (!available) {
        return 0;
    }
    size_t length = 0;
    while (true) {
        if (source.size() <= length + available) {
            source.resize(length + available);
        }
        length += stream.readBytes(&source[length], available);
        vTaskDelayMS(1);
        if (available = stream.available()) {
            continue;
        }
        vTaskDelayMS(10);
        if (available = stream.available()) {
            continue;
        }
        char c = source[length - 1];
        if (c == '\n' || c == '\r' || c == '\3' || c == '\4') {
            source[length - 1] = '\n';
            break;
        }
        vTaskDelayMS(100);
        if (available = stream.available()) {
            continue;
        }
        vTaskDelayMS(888);
        if (available = stream.available()) {
            continue;
        }
        break;
    }
    return length;
}

void Reader::task() {
#ifdef ROBOT_HAS_PROXY_UART
    ProxySerial.begin(115200, SERIAL_8N1, ROBOT_PROXY_RX_PIN, ROBOT_PROXY_TX_PIN);
#endif
    std::string source;
    size_t length;
    while (true) {
        length = read(Serial, source);
#ifdef ROBOT_HAS_OTA_UART
        if (!length) {
            length = read(otaUart, source);
        }
#endif
        if (length) {
#ifdef ROBOT_HAS_PROXY_UART
            ProxySerial.write(source.c_str(), length);
#endif
#ifdef ROBOT_HAS_PYTHON
            python.run(source.c_str(), length);
#endif
#ifdef ROBOT_HAS_SCRIPT
            script.run(source.c_str(), length);
#endif
            vTaskDelayMS(1);
        } else {
            vTaskDelayMS(100);
        }
    }
}

void Reader::task(void* arg) {
    reader.task();
}

#endif
