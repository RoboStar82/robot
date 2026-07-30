
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "config.h"

typedef struct {
    struct {
        bool value = false;
    } power;
    struct {
        bool value = false;
    } ble;
    struct {
        bool value = false;
    } wifi;
    struct {
        bool errors = false;
        bool sending = false;
        bool sleeping = false;
    } lora;
    struct {
        char button = 0;
    } controller;
} led_state_t;

typedef struct {
    struct {
        struct {
            bool value = false;
            unsigned int period = 199;
            unsigned int counter = 0;
        } state;
    } power;
    struct {
        struct {
            bool value = false;
            unsigned int period = 199;
            unsigned int counter = 0;
        } state;
    } ble;
    struct {
        struct {
            bool value = false;
            unsigned int period = 199;
            unsigned int counter = 0;
        } state;
    } wifi;
    struct {
        struct {
            bool value = false;
            unsigned int period = 19;
            unsigned int counter = 0;
        } errors;
        struct {
            bool value = false;
            unsigned int period = 199;
            unsigned int counter = 0;
        } sleeping;
    } lora;
} led_timers_t;

class Led {
   public:
    Led();
    ~Led();

    void begin();

    void onChange();

    void setPower(bool value);

    void setBLE(bool value);
    void setWiFi(bool value);

    void setLoraErrors(bool value);
    void setLoraSending(bool value);
    void setLoraSleeping(bool value);

    void setControllerButton(char value);

    void task();

   protected:
    uint8_t rgbPin = 0;
    uint8_t ledPin = 0;
    led_timers_t timers;
    led_state_t state;
    bool changed = false;
    int counter = 0;

    static void task(void* arg);
};

extern Led led;
