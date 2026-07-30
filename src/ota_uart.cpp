
#include "config.h"

#ifdef ROBOT_HAS_OTA_UART

#include "ota_uart.h"

OTAUart otaUart;

OTAUart::OTAUart() {}

OTAUart::~OTAUart() {}

void OTAUart::begin() {
    if (!startedTask) {
        xTaskCreate(task, "ota_uart_task", 8192, NULL, 1, &startedTask);
    }
}

void OTAUart::end() {
    if (startedTask) {
        vTaskDelete(startedTask);
    }
}

size_t OTAUart::printf(const char* format, ...) {
    if (!client) {
        return 0;
    }
    va_list args;
    va_start(args, format);
    size_t r = client.vprintf(format, args);
    va_end(args);
    return r;
}

void OTAUart::task() {
    server.begin();
    while (true) {
        if (client = server.accept()) {
            print("[OTA UART] Accept %s\n", client.localIP().toString().c_str());
            while (client.connected()) {
                if (client.available()) {
                    char c = client.read();
                    Serial.write(c);
                } else {
                    vTaskDelay(100);
                }
            }
            client.stop();
            print("[OTA UART] Stop\n");
        } else {
            vTaskDelay(100);
        }
    }
}

void OTAUart::task(void* arg) {
    otaUart.task();
}

#endif
