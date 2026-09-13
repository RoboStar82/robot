
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_OTA_UART

#include "ota.h"
#include "delay.h"
#ifdef ROBOT_HAS_WRITER
#include "writer.h"
#endif

OTAUart otaUart;

OTAUart::OTAUart() : Stream() {
    server.setNoDelay(true);
}

void OTAUart::begin() {
    if (!taskHandle) {
        server.begin();
        IPAddress ip = ota.getIP();
#ifdef ROBOT_HAS_WRITER
        writer.printf("[UART] monitor_port = socket://%s:%d\n", NET_HOSTNAME, ROBOT_OTA_UART_PORT);
        writer.printf("[UART] monitor_port = socket://%s:%d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
        writer.printf("[UART] nc %s %d\n", NET_HOSTNAME, ROBOT_OTA_UART_PORT);
        writer.printf("[UART] nc %s %d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
#else
        printf("[UART] monitor_port = socket://%s:%d\n", NET_HOSTNAME, ROBOT_OTA_UART_PORT);
        printf("[UART] monitor_port = socket://%s:%d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
        printf("[UART] nc %s %d\n", NET_HOSTNAME, ROBOT_OTA_UART_PORT);
        printf("[UART] nc %s %d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
#endif
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

bool OTAUart::connected() {
    return client.connected();
}

void OTAUart::task() {
    while (true) {
        if (server.hasClient()) {
            client = server.accept();
#ifdef ROBOT_HAS_WRITER
            writer.printf("[UART] begin: %s\n", client.remoteIP().toString().c_str());
#else
            printf("[UART] begin: %s\n", client.remoteIP().toString().c_str());
#endif
            while (client) {
                vTaskDelayMS(100);
            }
#ifdef ROBOT_HAS_WRITER
            writer.printf("[UART] end\n");
#else
            printf("[UART] end\n");
#endif
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
