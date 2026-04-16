
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
    esp_reset_reason_t resetReason = esp_reset_reason();
    log_i("Reset cod: %d\r\n", resetReason);

    switch (resetReason) {
    case ESP_RST_POWERON:
        settings.addResetReason("Power on");
        break;
    case ESP_RST_BROWNOUT:
        settings.addResetReason("Brown out");
        break;
    case ESP_RST_SW:
        settings.addResetReason("Software reset");
        break;
    case ESP_RST_PANIC:
        settings.addResetReason("Panic reset");
        break;
    case ESP_RST_INT_WDT:
        settings.addResetReason("INT WDT reset");
        break;
    case ESP_RST_TASK_WDT:
        settings.addResetReason("Task WDT reset");
        break;
    default:
        settings.addResetReason("other reset: " + resetReason);
        break;
    }
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
