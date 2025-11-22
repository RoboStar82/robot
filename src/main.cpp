
#include "controller.h"
#include "led.h"
#include "lora.h"
#include "navigation.h"
#include "ota.h"
#include "robot.h"
#include "settings.h"
#include "usb.h"
#include "version.h"

void setup() {
    Serial.begin(115200);
    log_i("Firmware: %u (%s)", BUILD_TIMESTAMP, BUILD_DATETIME);
    settings.begin();
#if ROBOT_HAS_CONTROLLER_USB
    usb.begin();
#endif
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
    lora.begin();
#endif
#if ROBOT_HAS_CONTROLLER_SERIAL
    controller.begin();
#endif
#if ROBOT_HAS_CONTROLLER_SERIAL || ROBOT_HAS_NAVIGATION_SERIAL
    navigation.begin();
#endif
#ifdef RGB_BUILTIN
    led.begin();
#elifdef LED_BUILTIN
    led.begin();
#endif
    robot.begin();
    ota.begin();
}

void loop() {
    delay(1000);
}
