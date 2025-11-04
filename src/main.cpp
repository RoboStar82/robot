
#include "led.h"
#include "lora.h"
#include "ota.h"
#include "robot.h"
#include "settings.h"
#include "usb.h"

void setup() {
    Serial.begin(115200);
    settings.begin();
#if ROBOT_HAS_CONTROLLER_USB
    usb.begin();
#endif
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
    lora.begin();
#endif
#if ROBOT_HAS_LED || ROBOT_HAS_RGB_LED
    led.begin();
#endif
    robot.begin();
    ota.begin();
}

void loop() {
    delay(1000);
}
