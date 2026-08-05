
#include "config.h"

#ifdef ROBOT_HAS_OTA_UART

#include "ota.h"

OTAUart otaUart;

OTAUart::OTAUart() : Stream() {
    server.setNoDelay(true);
}

void OTAUart::begin() {
    if (!taskStarted) {
        server.begin();
        xTaskCreate(task, "ota_uart_task", 8192, NULL, 1, &taskStarted);
        IPAddress ip = ota.getIP();
        printf("[UART] monitor_port = socket://%s:%d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
        printf("[UART] nc %s %d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
    }
}

void OTAUart::begin(unsigned long baud) {
    Serial.begin(baud);
    stdoutReplaced = _GLOBAL_REENT->_stdout;
    _GLOBAL_REENT->_stdout = funopen(NULL, NULL, write, NULL, NULL);
    setvbuf(_GLOBAL_REENT->_stdout, NULL, _IONBF, 0);
}

void OTAUart::end() {
    if (taskStarted) {
        vTaskDelete(taskStarted);
        server.end();
    }
}

int OTAUart::available() {
    return Serial.available() + client.available();
}

int OTAUart::peek() {
    if (Serial.available() > 0) {
        return Serial.peek();
    }
    if (client.available() > 0) {
        return client.peek();
    }
    return Serial.peek();
}

int OTAUart::read() {
    if (Serial.available() > 0) {
        return Serial.read();
    }
    if (client.available() > 0) {
        return client.read();
    }
    return Serial.read();
}

size_t OTAUart::write(uint8_t c) {
    BaseType_t lock = xSemaphoreTake(writeLock, waitLock);
    size_t r = Serial.write(c);
    if (lock != pdTRUE) {
        return r;
    }
    if (txLength < sizeof(txBuffer)) {
        txBuffer[txLength] = c;
        txLength++;
    } else {
        memmove(txBuffer, txBuffer + 1, sizeof(txBuffer) - 1);
        txBuffer[txLength - 1] = c;
    }
    xSemaphoreGive(writeLock);
    if (txLength >= sizeof(txBuffer) || c == '\n') {
        flush();
    }
    return r;
}

size_t OTAUart::write(const uint8_t* buffer, size_t length) {
    if (!length) {
        return length;
    }
    BaseType_t lock = xSemaphoreTake(writeLock, waitLock);
    size_t r = Serial.write(buffer, length);
    if (lock != pdTRUE) {
        return r;
    }
    if (length >= sizeof(txBuffer)) {
        memcpy(txBuffer, buffer + length - sizeof(txBuffer), sizeof(txBuffer));
        txLength = sizeof(txBuffer);
    } else if (txLength + length > sizeof(txBuffer)) {
        memmove(txBuffer, txBuffer + txLength - (sizeof(txBuffer) - length), sizeof(txBuffer) - length);
        txLength = sizeof(txBuffer);
    } else {
        memcpy(txBuffer + txLength, buffer, length);
        txLength += length;
    }
    xSemaphoreGive(writeLock);
    if (txLength >= sizeof(txBuffer) || buffer[length - 1] == '\n') {
        flush();
    }
    return r;
}

void OTAUart::flush() {
    BaseType_t lock = xSemaphoreTake(writeLock, waitLock);
    Serial.flush();
    if (lock != pdTRUE) {
        return;
    }
    while (txLength > 0) {
        if (client) {
            size_t length = client.write(txBuffer, txLength);
            if (length > 0) {
                memmove(txBuffer, txBuffer + length, txLength - length);
                txLength -= length;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    xSemaphoreGive(writeLock);
}

int OTAUart::write(void* cookie, const char* buffer, int length) {
    return otaUart.write(buffer, length);
}

void OTAUart::task() {
    while (true) {
        if (server.hasClient()) {
            client = server.accept();
            printf("[UART] begin: %s\n", client.remoteIP().toString().c_str());
            while (client) {
                delay(1000);
            }
            printf("[UART] end\n");
            client.stop();
        } else {
            delay(1000);
        }
    }
}

void OTAUart::task(void* arg) {
    otaUart.task();
}

#endif
