
#pragma once

#include <Arduino.h>

typedef struct {
    int8_t lx : 4 = 0;
    int8_t ly : 4 = 0;
    int8_t rx : 4 = 0;
    int8_t ry : 4 = 0;
    int8_t dx : 2 = 0;
    int8_t dy : 2 = 0;
    int8_t lz : 2 = 0;
    int8_t rz : 2 = 0;
    bool lt : 1 = 0;
    bool rt : 1 = 0;
    bool a : 1 = 0;
    bool b : 1 = 0;
    bool x : 1 = 0;
    bool y : 1 = 0;
    bool start : 1 = 0;
    bool back : 1 = 0;
    uint8_t mode : 8 = 0;
} __attribute__((packed)) controller_state_t;

class Controller {
   public:
    controller_state_t getState();

    void getState(uint8_t state[]);

    void getState(controller_state_t* state);

    void setState(controller_state_t newState);

    void setState(uint8_t newState[]);

    void setState(controller_state_t* newState);

    void onChange(controller_state_t oldState);

    void onChangeLora();
    void onChangeRobot(controller_state_t oldState);

    void print();

   protected:
    controller_state_t state;
};

extern Controller controller;
