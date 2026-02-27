
#include "controller.h"
#include "led.h"
#include "ota.h"
#include "robot.h"
#include "settings.h"
#include "version.h"
#if ROBOT_HAS_CONTROLLER_USB
#include "usb.h"
#endif
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
#include "lora.h"
#endif
#if ROBOT_HAS_LIDAR
#include "lidar.h"
#endif
#if ROBOT_HAS_MOTOR_ENCODER_I2C
#include "encoder.h"
#endif
#if ROBOT_HAS_NAVIGATION_SENDER || ROBOT_HAS_NAVIGATION_SERIAL
#include "navigation.h"
#endif
#if ROBOT_HAS_IMU
#include "imu.h"
#endif

void setup() {
    Serial.begin(115200);
    log_i("%s (%s.local) Firmware: %u (%s)", BLE_DEVICE_NAME, NET_HOSTNAME, BUILD_TIMESTAMP, BUILD_DATETIME);
    settings.begin();
#if ROBOT_HAS_CONTROLLER_USB
    usb.begin();
#endif
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
    lora.begin();
#endif
#if ROBOT_HAS_CONTROLLER_SERIAL || ROBOT_HAS_TRANSCEIVER_SERIAL
    controller.begin();
#endif
#if ROBOT_HAS_LIDAR
    lidar.begin();
#endif
#if ROBOT_HAS_MOTOR_ENCODER_I2C
    encoder.begin();
#endif
#if ROBOT_HAS_NAVIGATION_SENDER || ROBOT_HAS_NAVIGATION_SERIAL
    navigation.begin();
#endif
#if ROBOT_HAS_IMU
    vTaskDelay(1000);
    imu.begin();
#endif
#ifdef RGB_BUILTIN
    led.begin();
#else
#ifdef LED_BUILTIN
    led.begin();
#endif
#endif
    ota.begin();
    robot.begin();
}

void loop() {
    vTaskDelay(1000);
}
