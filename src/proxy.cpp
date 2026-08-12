
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_PROXY

#include "delay.h"
#include "print.h"
#include "proxy.h"

Proxy proxy;

Proxy::Proxy() : Stream() {}

Proxy::~Proxy() {}

void Proxy::begin() {
    if (!taskStarted) {
        ProxySerial.begin(115200, SERIAL_8N1, ROBOT_PROXY_RX_PIN, ROBOT_PROXY_TX_PIN);
        xTaskCreate(task, "proxy_task", 4096, NULL, 1, &taskStarted);
    }
}

void Proxy::end() {
    if (taskStarted) {
        vTaskDelete(taskStarted);
        taskStarted = nullptr;
    }
}

int Proxy::available() {
    return ProxySerial.available();
}

int Proxy::peek() {
    return ProxySerial.peek();
}

int Proxy::read() {
    return ProxySerial.read();
}

size_t Proxy::write(uint8_t c) {
    return ProxySerial.write(c);
}

size_t Proxy::write(const uint8_t* buffer, size_t length) {
    return ProxySerial.write(buffer, length);
}

void Proxy::flush() {
    return ProxySerial.flush();
}

void Proxy::task() {
    while (true) {
        while (RobotSerial.available() > 0) {
            ProxySerial.write(RobotSerial.read());
        }
        while (ProxySerial.available() > 0) {
            RobotSerial.write(ProxySerial.read());
        }
        vTaskDelayMS(1000);
    }
}

void Proxy::task(void* arg) {
    proxy.task();
}

#endif
