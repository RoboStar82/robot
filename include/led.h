
#pragma once

#include <Arduino.h>

typedef struct {
    struct {
        bool on = false;
    } power;
    struct {
        char button = 0;
    } controller;
    struct {
        bool errors = false;
        bool sending = false;
        bool sleeping = false;
    } lora;
    struct {
        bool ble = false;
        bool wifi = false;
    } ota;
} led_state_t;

typedef struct {
    struct {
        struct {
            bool value = false;
            unsigned int period = 199;
            unsigned int counter = 0;
        } on;
    } power;
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
    struct {
        struct {
            bool value = false;
            unsigned int period = 399;
            unsigned int counter = 0;
        } ble;
        struct {
            bool value = false;
            unsigned int period = 399;
            unsigned int counter = 0;
        } wifi;
    } ota;
} led_timers_t;

class Led {
   public:
    void begin();

    void onChange();

    void setPowerOn(bool value);

    void setControllerButton(char value);

    void setLoraErrors(bool value);
    void setLoraSending(bool value);
    void setLoraSleeping(bool value);

    void setOtaBle(bool value);
    void setOtaWiFi(bool value);

    void task();

   protected:
    led_timers_t timers;
    led_state_t state;
    bool changed = false;
    int counter = 0;

    static void task(void* arg);
};

extern Led led;
