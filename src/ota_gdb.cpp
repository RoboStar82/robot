
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_OTA_GDB

#include "ota.h"
#include "delay.h"
#include "print.h"

OTAGdb otaGdb;

OTAGdb::OTAGdb() {}

void OTAGdb::begin() {
    if (!taskMainHandle) {
        xTaskCreate(taskMain, "gdb_main", 4096, NULL, 1, &taskMainHandle);
    }
    if (!taskMainHandle) {
        xTaskCreate(taskServer, "gdb_server", 4096, NULL, 1, &taskMainHandle);
    }
    IPAddress ip = ota.getIP();
    print("[GDB] ~/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gdb\n");
    print("[GDB] target extended-remote %s:%d\n", NET_HOSTNAME, ROBOT_OTA_GDB_PORT);
    print("[GDB] target extended-remote %s:%d\n", ip.toString().c_str(), ROBOT_OTA_GDB_PORT);
}

void OTAGdb::end() {}

void OTAGdb::write(char c, bool flush) {
    txBuffer[txLength] = c;
    txLength++;
    if (txLength >= sizeof(txBuffer) || flush) {
        this->flush(flush);
    }
}

void OTAGdb::flush(bool flush) {
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

void OTAGdb::taskServer() {
    server.setNoDelay(true);
    server.begin();
    while (true) {
        if (server.hasClient()) {
            client = server.accept();
            print("[GDB] begin: %s\n", client.remoteIP().toString().c_str());
            while (client) {
                vTaskDelayMS(100);
            }
            print("[GDB] end\n");
            client.stop();
        } else {
            vTaskDelayMS(100);
        }
    }
}

void OTAGdb::taskMain(void* arg) {
    while (true) {
        platform_main();
        vTaskDelayMS(1);
    }
}

void OTAGdb::taskServer(void* arg) {
    otaGdb.taskServer();
}

int gdb_if_init(void) {
    return 0;
}

char gdb_if_getchar(void) {
    while (!otaGdb.client.connected()) {
        vTaskDelayMS(100);
    }
    while (otaGdb.client.available() <= 0) {
        vTaskDelayMS(10);
        if (!otaGdb.client.connected()) {
            return 0;
        }
    }
    int c = otaGdb.client.read();
    return c < 0 ? 0 : c;
}

char gdb_if_getchar_to(uint32_t timeout) {
    if (otaGdb.client.available() > 0) {
        return otaGdb.client.read();
    }
    while (timeout > 0) {
        vTaskDelayMS(1);
        if (otaGdb.client.available() > 0) {
            return otaGdb.client.read();
        }
        timeout--;
    }
    return -1;
}

void gdb_if_putchar(char c, bool flush) {
    return otaGdb.write(c, flush);
}

void gdb_if_flush(bool force) {
    return otaGdb.flush(force);
}

void debug_serial_send_stdout(const uint8_t* buffer, size_t length) {
    print(buffer, length);
}

void platform_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprint(format, args);
    va_end(args);
}

#endif
