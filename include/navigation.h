
#pragma once

#include <Arduino.h>

#if ROBOT_HAS_NAVIGATION_SENDER || ROBOT_HAS_NAVIGATION_SERIAL
#define NavigationSerial Serial2
#endif

class Navigation {
   public:
    Navigation();
    ~Navigation();

    void begin();

    void task();

    static void task(void* arg);
};

extern Navigation navigation;
