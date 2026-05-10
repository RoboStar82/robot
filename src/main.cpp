
#include <Arduino.h>

#include "display.h"
#include "version.h"

void setup() {
    Serial.begin(115200);
    Serial.printf("%s (%s.local) Firmware: %u (%s)", BLE_DEVICE_NAME, NET_HOSTNAME, BUILD_TIMESTAMP, BUILD_DATETIME);
    display.begin();
}

void loop() {
    delay(1000);
}
