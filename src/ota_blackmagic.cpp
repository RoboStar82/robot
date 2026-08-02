
#include "config.h"

#ifdef ROBOT_HAS_OTA_BLACKMAGIC

#include "ota_blackmagic.h"

OTABlackMagic otaBlackMagic;

OTABlackMagic::OTABlackMagic() {}

OTABlackMagic::~OTABlackMagic() {}

void OTABlackMagic::begin() {
    if (!taskMainStarted) {
        xTaskCreate(taskMain, "gdb_main", 4096, NULL, 1, &taskMainStarted);
    }
    if (!taskServerStarted) {
        xTaskCreate(taskServer, "gdb_server", 4096, NULL, 1, &taskServerStarted);
    }
    IPAddress ip;
    wifi_mode_t wifiMode = WiFi.getMode();
    if (wifiMode == WIFI_MODE_STA) {
        ip = WiFi.localIP();
    } else if (wifiMode == WIFI_MODE_AP) {
        ip = WiFi.softAPIP();
    }
    print("[OTA GDB] ~/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gdb\n");
    print("[OTA GDB] target extended-remote %s:%d\n", ip.toString().c_str(), ROBOT_OTA_BLACKMAGIC_PORT);
}

void OTABlackMagic::end() {}

void OTABlackMagic::write(char c, bool flush) {
    txBuffer[txLength] = c;
    txLength++;
    if (txLength >= sizeof(txBuffer) || flush) {
        this->flush(flush);
    }
}

void OTABlackMagic::flush(bool flush) {
    if (!txLength) {
        return;
    }
    size_t size = 0;
    while (size < txLength) {
        if (client.connected()) {
            size += client.write(txBuffer + size, txLength - size);
        } else {
            txLength = 0;
            return;
        }
    }
    txLength = 0;
}

void OTABlackMagic::taskMain() {
    while (true) {
        platform_main();
    }
}

void OTABlackMagic::taskServer() {
    server.begin();
    while (true) {
        if (client = server.accept()) {
            print("[OTA GDB] begin %s\n", client.remoteIP().toString().c_str());
            while (client.connected()) {
                vTaskDelay(100);
            }
            client.stop();
        } else {
            vTaskDelay(100);
        }
    }
}

void OTABlackMagic::taskMain(void* arg) {
    otaBlackMagic.taskMain();
}

void OTABlackMagic::taskServer(void* arg) {
    otaBlackMagic.taskServer();
}

int gdb_if_init(void) {
    return 0;
}

char gdb_if_getchar(void) {
    while (!otaBlackMagic.client.connected()) {
        vTaskDelay(100);
    }
    while (otaBlackMagic.client.available() <= 0) {
        vTaskDelay(10);
        if (!otaBlackMagic.client.connected()) {
            return 0;
        }
    }
    int c = otaBlackMagic.client.read();
    return c < 0 ? 0 : c;
}

char gdb_if_getchar_to(uint32_t timeout) {
    if (otaBlackMagic.client.available() > 0) {
        return otaBlackMagic.client.read();
    }
    while (timeout > 0) {
        vTaskDelay(1);
        if (otaBlackMagic.client.available() > 0) {
            return otaBlackMagic.client.read();
        }
        timeout--;
    }
    return -1;
}

void gdb_if_putchar(char c, bool flush) {
    return otaBlackMagic.write(c, flush);
}

void gdb_if_flush(bool force) {
    return otaBlackMagic.flush(force);
}

void debug_serial_send_stdout(const uint8_t* data, size_t len) {
    Serial.write(data, len);
}

void platform_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    Serial.vprintf(format, args);
    va_end(args);
}

#endif
