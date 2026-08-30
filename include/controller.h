
#pragma once

#include <Arduino.h>

#include "config.h"

typedef struct {
    uint16_t vendorId = 0x00;
    uint16_t productId = 0x00;
    size_t length = 0;
    uint8_t data[16] = {0};
} ControllerInput_t;

typedef struct {
    uint8_t LX;
    uint8_t LY;
    uint8_t RX;
    uint8_t RY;
    uint8_t D : 4;
    uint8_t X : 1;
    uint8_t A : 1;
    uint8_t B : 1;
    uint8_t Y : 1;
    uint8_t LB : 1;
    uint8_t RB : 1;
    uint8_t LT : 1;
    uint8_t RT : 1;
    uint8_t back : 1;
    uint8_t start : 1;
    uint8_t L3 : 1;
    uint8_t R3 : 1;
    uint8_t mode : 8;
} __attribute__((packed)) ControllerInput_046d_0216_t;

typedef struct {
    uint8_t _;
    uint8_t LX;
    uint8_t LY;
    uint8_t RX;
    uint8_t RY;
    uint8_t D : 4;
    uint8_t X : 1;
    uint8_t A : 1;
    uint8_t B : 1;
    uint8_t Y : 1;
    uint8_t LB : 1;
    uint8_t RB : 1;
    uint8_t LT : 1;
    uint8_t RT : 1;
    uint8_t back : 1;
    uint8_t start : 1;
    uint8_t L3 : 1;
    uint8_t R3 : 1;
    uint8_t mode : 8;
} __attribute__((packed)) ControllerInput_046d_0219_t;

typedef struct {
    uint8_t D = 0xf;
    uint8_t LX = 0;
    uint8_t LY = 0;
    uint8_t RX = 0;
    uint8_t RY = 0;
    uint8_t RA = 0;
    uint8_t LA = 0;
    uint8_t A : 1 = 0;
    uint8_t B : 1 = 0;
    uint8_t L4 : 1 = 0;
    uint8_t X : 1 = 0;
    uint8_t Y : 1 = 0;
    uint8_t R4 : 1 = 0;
    uint8_t LB : 1 = 0;
    uint8_t RB : 1 = 0;
    uint8_t LT : 1 = 0;
    uint8_t RT : 1 = 0;
    uint8_t back : 1 = 0;
    uint8_t start : 1 = 0;
    uint8_t mode : 1 = 0;
    uint8_t L3 : 1 = 0;
    uint8_t R3 : 1 = 0;
} __attribute__((packed)) ControllerInput_2dc8_301b_t;

typedef struct {
    int8_t LX = 0; // 8
    int8_t LY = 0; // 16
    int8_t RX = 0; // 24
    int8_t RY = 0; // 32
    int8_t DX : 2 = 0; // 34
    int8_t DY : 2 = 0; // 36
    bool A : 1 = false; // 37
    bool B : 1 = false; // 38
    bool X : 1 = false; // 39
    bool Y : 1 = false; // 40
    bool LT : 1 = false; // 41
    bool RT : 1 = false; // 42
    bool LB : 1 = false; // 43
    bool RB : 1 = false; // 44
    bool L3 : 1 = false; // 45
    bool R3 : 1 = false; // 46
    bool L4 : 1 = false; // 47
    bool R4 : 1 = false; // 48
    uint8_t LA : 4 = 0; // 52
    uint8_t RA : 4 = 0; // 56
    bool start : 1 = false; // 57
    bool back : 1 = false; // 58
    uint8_t mode : 6 = 0; // 64
} __attribute__((packed)) ControllerState_t;

class Controller {
   public:
    Controller();
    virtual ~Controller() = default;

    void begin();

    void end();

    void onInput(uint8_t* data, size_t length, uint16_t vendorId, uint16_t productId);

    bool setState(ControllerState_t newState);

    void onChange(ControllerState_t oldState);

    void printState(ControllerState_t state);

    void task();

   protected:
    ControllerInput_t input;
    ControllerState_t state;

    int8_t mapDX[16] = {0, 1, 1, 1, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
    int8_t mapDY[16] = {1, 1, 0, -1, -1, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    int8_t easeInOut64[256] = {-64, -64, -64, -64, -64, -64, -64, -64, -64, -63, -63, -63, -63, -63, -62, -62, -62, -62, -61, -61, -61, -60, -60, -60, -59, -59, -59, -58, -58, -57, -57, -56, -56, -55, -54, -54, -53, -53, -52, -51, -51, -50, -49, -49, -48, -47, -46, -46, -45, -44, -43, -43, -42, -41, -40, -39, -38, -38, -37, -36, -35, -34, -33, -32, -32, -31, -30, -29, -28, -27, -26, -26, -25, -24, -23, -22, -21, -21, -20, -19, -18, -18, -17, -16, -15, -15, -14, -13, -13, -12, -11, -11, -10, -10, -9, -8, -8, -7, -7, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -2, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63, 63, 64, 64, 64, 64, 64, 64, 64, 64, 64};

    static inline void task(void* arg);
};

extern Controller controller;