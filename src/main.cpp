
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"
#include "delay.h"
#include "print.h"
#include "version.h"

#ifdef ROBOT_HAS_BLE
#include "ble.h"
#endif

#ifdef ROBOT_HAS_OTA
#include "ota.h"
#endif

#ifdef ROBOT_HAS_USB
#include "usb.h"
#endif

#ifdef ROBOT_HAS_LED
#include "led.h"
#endif

#ifdef ROBOT_HAS_DISPLAY
#include "display.h"
#endif

#ifdef ROBOT_HAS_PROXY
#include "proxy.h"
#endif

#ifdef ROBOT_HAS_SETTINGS
#include "settings.h"
#endif

#ifdef ROBOT_HAS_SCRIPT
#include "script.h"
#endif

#ifdef ROBOT_HAS_CAMERA
#include "camera.h"
#endif

#ifdef ROBOT_HAS_TFLM
#include "tflm.h"
#endif

void setup() {
    RobotSerial.begin(115200);
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
#ifdef ROBOT_HAS_USB
    usb.begin();
#endif
#ifdef ROBOT_HAS_PROXY
    proxy.begin();
#endif
#ifdef ROBOT_HAS_SCRIPT
    script.begin();
#endif
#ifdef ROBOT_HAS_CAMERA
    camera.begin();
#endif
#ifdef ROBOT_HAS_TFLM
    tflm.begin();
#endif
#ifdef ARDUINO_STM32
    vTaskStartScheduler();
#endif
}

void loop() {
#ifdef ARDUINO_ESP32
    vTaskDelayMS(1000);
#endif
}
