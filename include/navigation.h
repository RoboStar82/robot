
#pragma once

#include <Arduino.h>

#if ROBOT_HAS_NAVIGATION_SERIAL
#define NavigationSerial Serial1
#endif

class Navigation {
   public:
    void begin();

    void sendControllerState();

    void task();

    static void task(void* arg);
};

extern Navigation navigation;
