
#include "config.h"

#ifdef ROBOT_HAS_OTA_UART

#include "ota_uart.h"

#ifdef ROBOT_HAS_SCRIPT
#include "script.h"
#endif

OTAUart otaUart;

OTAUart::OTAUart() {}

OTAUart::~OTAUart() {}

void OTAUart::begin() {
    if (!taskStarted) {
        server.begin();
        xTaskCreate(task, "ota_uart_task", 8192, NULL, 1, &taskStarted);
        IPAddress ip;
        wifi_mode_t wifiMode = WiFi.getMode();
        if (wifiMode == WIFI_MODE_STA) {
            ip = WiFi.localIP();
        } else if (wifiMode == WIFI_MODE_AP) {
            ip = WiFi.softAPIP();
        }
        print("[OTA UART] monitor_port = socket://%s:%d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
        print("[OTA UART] nc %s %d\n", ip.toString().c_str(), ROBOT_OTA_UART_PORT);
    }
}

void OTAUart::end() {
    if (taskStarted) {
        vTaskDelete(taskStarted);
        server.end();
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
    while (true) {
        if (client = server.accept()) {
            print("[OTA UART] begin %s\n", client.remoteIP().toString().c_str());
            std::string source;
            size_t length = 0;
            unsigned long start = 0;
#ifdef ROBOT_HAS_SCRIPT
            script_write_callback write = [this](const uint8_t* buffer, size_t length) {
                if (buffer && length) {
                    this->client.write(buffer, length);
                }
            };
#endif
            while (client.connected()) {
                if (client.available() > 0) {
                    start = millis();
                    source.resize(length + client.available());
                    while (client.available() > 0) {
                        if (length >= source.length()) {
                            source.resize(length + client.available());
                        }
                        source[length] = client.read();
                        length++;
                    }
                    if (length > 1 && source[length - 1] == '\n') {
#ifdef ROBOT_HAS_SCRIPT
                        script.run({
                            .filename = "<OTA UART>",
                            .source = source.c_str(),
                            .length = length,
                            .write = write,
                        });
#endif
                        length = 0;
                    } else {
                        vTaskDelay(1);
                    }
                } else if (length) {
                    if (millis() - start >= 1000) {
#ifdef ROBOT_HAS_SCRIPT
                        script.run({
                            .filename = "<OTA UART>",
                            .source = source.c_str(),
                            .length = length,
                            .write = write,
                        });
#endif
                        length = 0;
                    } else {
                        vTaskDelay(10);
                    }
                } else {
                    vTaskDelay(100);
                }
            }
            client.stop();
            print("[OTA UART] end\n");
        } else {
            vTaskDelay(1000);
        }
    }
}

void OTAUart::task(void* arg) {
    otaUart.task();
}

#endif
