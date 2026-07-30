
#include <Arduino.h>

#ifdef ARDUINO_STM32
#include <FreeRTOS.h>
#include <task.h>
#endif

#include "config.h"
#include "print.h"
#include "version.h"

#ifdef ROBOT_HAS_BLE
#include "ble.h"
#endif

#ifdef ROBOT_HAS_OTA
#include "ota.h"
#endif

#ifdef ROBOT_HAS_LED
#include "led.h"
#endif

#ifdef ROBOT_HAS_DISPLAY
#include "display.h"
#endif

#ifdef ROBOT_HAS_SETTINGS
#include "settings.h"
#endif

#ifdef ROBOT_HAS_SCRIPT
#include "script.h"
#endif

void setup() {
    Serial.begin(115200);
    print("[main] %s (%s.local) Firmware: %u (%s)\n", BLE_DEVICE_NAME, NET_HOSTNAME, BUILD_TIMESTAMP, BUILD_DATETIME);
#ifdef ROBOT_HAS_LED
    led.begin();
#endif
#ifdef ROBOT_HAS_DISPLAY
    display.begin();
#endif
#ifdef ROBOT_HAS_SETTINGS
    settings.begin();
#endif
#ifdef ROBOT_HAS_BLE
    ble.begin();
#endif
#ifdef ROBOT_HAS_OTA
    ota.begin();
#endif
#ifdef ROBOT_HAS_SCRIPT
    script.begin();
#endif
}

void loop() {
    vTaskDelay(1000);
}
