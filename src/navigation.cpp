
#include "navigation.h"

#include "controller.h"

Navigation navigation;

void Navigation::begin() {
#if ROBOT_HAS_NAVIGATION_SERIAL
    NavigationSerial.begin(115200);
#endif
    xTaskCreate(task, "navigation_task", 4096, NULL, 1, NULL);
}

void Navigation::sendControllerState() {
#if ROBOT_HAS_NAVIGATION_SERIAL
    uint8_t data[8];
    controller.getState(data);
    NavigationSerial.printf("CTRL=%02x%02x%02x%02x%02x%02x%02x%02x\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
#endif
}

void Navigation::task() {
#if ROBOT_HAS_NAVIGATION_SERIAL
    while (true) {
        if (NavigationSerial.available()) {
            uint8_t data[32];
            size_t length = NavigationSerial.readBytesUntil('\n', data, 31);
            data[length] = '\0';
            Serial.print((char*)data);
        } else {
            delay(1000);
        }
    }
#else
    while (true) {
        delay(1000);
    }
#endif
}

void Navigation::task(void* arg) {
    navigation.task();
}
