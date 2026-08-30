
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_OTA_UART

#include "ota.h"
#include "delay.h"
#include "print.h"

OTAUart otaUart;

OTAUart::OTAUart() : Stream() {
    server.setNoDelay(true);
}

void OTAUart::begin() {
    if (!taskHandle) {
        server.begin();
        IPAddress ip = ota.getIP();
        printf("[UART] monitor_port = socket://%s:%d\n", NET_HOSTNAME, ROBOT_OTA_UART_PORT);
        printf("[UART] monitor_port = socket://%s:%d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
        printf("[UART] nc %s %d\n", NET_HOSTNAME, ROBOT_OTA_UART_PORT);
        printf("[UART] nc %s %d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
        xTaskCreate(task, "ota_uart_task", 8192, NULL, 1, &taskHandle);
    }
}

void OTAUart::end() {
    if (taskHandle) {
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
        server.end();
    }
}

int OTAUart::available() {
    return client.available();
}

int OTAUart::peek() {
    return client.peek();
}

int OTAUart::read() {
    return client.read();
}

size_t OTAUart::write(uint8_t c) {
    return client.write(c);
}

size_t OTAUart::write(const uint8_t* buffer, size_t length) {
    return client.write(buffer, length);
}

void OTAUart::flush() {
    return client.flush();
}

void OTAUart::task() {
    while (true) {
        if (server.hasClient()) {
            client = server.accept();
            printf("[UART] begin: %s\n", client.remoteIP().toString().c_str());
            while (client) {
                vTaskDelayMS(100);
            }
            printf("[UART] end\n");
            client.stop();
        } else {
            vTaskDelayMS(100);
        }
    }
}

void OTAUart::task(void* arg) {
    otaUart.task();
}

#endif
