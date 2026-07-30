
#include <Arduino.h>

#include "display.h"
#include "version.h"

void setup() {
    Serial.begin(115200);
    Serial.printf("[main] %s (%s.local) Firmware: %u (%s)\n", BLE_DEVICE_NAME, NET_HOSTNAME, BUILD_TIMESTAMP, BUILD_DATETIME);
    display.begin();
}

void loop() {
    delay(1000);
}
