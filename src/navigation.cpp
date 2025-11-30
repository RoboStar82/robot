
#include "navigation.h"

#include "controller.h"

Navigation navigation;

Navigation::Navigation() {}

Navigation::~Navigation() {}

void Navigation::begin() {
#if (ROBOT_HAS_NAVIGATION_ROLE || ROBOT_HAS_NAVIGATION_SERIAL) && !(ROBOT_HAS_CONTROLLER_SERIAL || ROBOT_HAS_TRANSCEIVER_SERIAL)
    NavigationSerial.begin(115200, SERIAL_8N1, 19, 20);
#endif
#if ROBOT_HAS_NAVIGATION_ROLE || ROBOT_HAS_NAVIGATION_SERIAL
    xTaskCreate(task, "navigation_task", 4096, NULL, 1, NULL);
#endif
}

void Navigation::task() {
#if ROBOT_HAS_NAVIGATION_ROLE
    while (true) {
        delay(1000);
    }
#endif
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
#endif
}

void Navigation::task(void* arg) {
    navigation.task();
}
