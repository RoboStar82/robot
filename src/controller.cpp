
#include <Arduino.h>

#include "config.h"

#ifdef ROBOT_HAS_CONTROLLER

#include "controller.h"
#include "print.h"

Controller controller;

Controller::Controller() {}

void Controller::begin() {}

void Controller::end() {}

void Controller::onInput(uint8_t* data, size_t length, uint16_t vendorId, uint16_t productId) {
    if (input.vendorId == vendorId && input.productId == productId && input.length == length) {
        if (memcmp(input.data, data, length) == 0) {
            return;
        }
    }
    input.vendorId = vendorId;
    input.productId = productId;
    input.length = length;
    memcpy(input.data, data, length);
#ifdef DEBUG_CONTROLLER
    print("[controller] input %04x:%04x %d: 0x", vendorId, productId, length);
    for (int i = 0; i < length; i++) {
        print("%02x", data[i]);
    }
    print("\n");
#endif
    ControllerState_t state;
    if (vendorId == 0x2dc8 && productId == 0x301b) {
        ControllerInput_2dc8_301b_t* input = (ControllerInput_2dc8_301b_t*)data;
        state = {
            .LX = easeInOut64[input->LX],
            .LY = easeInOut64[0xff - input->LY],
            .RX = easeInOut64[input->RX],
            .RY = easeInOut64[0xff - input->RY],
            .DX = mapDX[input->D],
            .DY = mapDY[input->D],
            .A = input->A ? true : false,
            .B = input->B ? true : false,
            .X = input->X ? true : false,
            .Y = input->Y ? true : false,
            .LT = input->LT ? true : false,
            .RT = input->RT ? true : false,
            .LB = input->LB ? true : false,
            .RB = input->RB ? true : false,
            .L3 = input->L3 ? true : false,
            .R3 = input->R3 ? true : false,
            .L4 = input->L4 ? true : false,
            .R4 = input->R4 ? true : false,
            .LA = (uint8_t)(input->LA >> 4),
            .RA = (uint8_t)(input->RA >> 4),
            .start = input->start ? true : false,
            .back = input->back ? true : false,
            .mode = input->mode,
        };
    }
    if (vendorId == 0x046d && productId == 0x0216) {
        ControllerInput_046d_0216_t* input = (ControllerInput_046d_0216_t*)data;
        state = {
            .LX = easeInOut64[input->LX],
            .LY = easeInOut64[0xff - input->LY],
            .RX = easeInOut64[input->RX],
            .RY = easeInOut64[0xff - input->RY],
            .DX = mapDX[input->D],
            .DY = mapDY[input->D],
            .A = input->A ? true : false,
            .B = input->B ? true : false,
            .X = input->X ? true : false,
            .Y = input->Y ? true : false,
            .LT = input->LT ? true : false,
            .RT = input->RT ? true : false,
            .LB = input->LB ? true : false,
            .RB = input->RB ? true : false,
            .L3 = input->L3 ? true : false,
            .R3 = input->R3 ? true : false,
            .L4 = false,
            .R4 = false,
            .LA = (uint8_t)(input->LT ? 0xf : 0),
            .RA = (uint8_t)(input->RT ? 0xf : 0),
            .start = input->start ? true : false,
            .back = input->back ? true : false,
            .mode = input->mode,
        };
    }
    if (vendorId == 0x046d && productId == 0x0219) {
        ControllerInput_046d_0219_t* input = (ControllerInput_046d_0219_t*)data;
        state = {
            .LX = easeInOut64[input->LX],
            .LY = easeInOut64[0xff - input->LY],
            .RX = easeInOut64[input->RX],
            .RY = easeInOut64[0xff - input->RY],
            .DX = mapDX[input->D],
            .DY = mapDY[input->D],
            .A = input->A ? true : false,
            .B = input->B ? true : false,
            .X = input->X ? true : false,
            .Y = input->Y ? true : false,
            .LT = input->LT ? true : false,
            .RT = input->RT ? true : false,
            .LB = input->LB ? true : false,
            .RB = input->RB ? true : false,
            .L3 = input->L3 ? true : false,
            .R3 = input->R3 ? true : false,
            .L4 = false,
            .R4 = false,
            .LA = (uint8_t)(input->LT ? 0xf : 0),
            .RA = (uint8_t)(input->RT ? 0xf : 0),
            .start = input->start ? true : false,
            .back = input->back ? true : false,
            .mode = input->mode,
        };
    }
    setState(state);
}

bool Controller::setState(ControllerState_t newState) {
    if (memcmp(&newState, &state, sizeof(ControllerState_t))) {
        ControllerState_t oldState = state;
        state = newState;
        onChange(oldState);
        return true;
    }
    return false;
}

void Controller::onChange(ControllerState_t oldState) {
    printState(state);
}

void Controller::printState(ControllerState_t state) {
    size_t length = 0;
    char message[256] = {0};
    length += snprintf(message + length, sizeof(message) - length, "[controller] ");
    bool none = true;
    if (state.LX) {
        length += snprintf(message + length, sizeof(message) - length, "LX=%d ", state.LX);
        none = false;
    }
    if (state.LY) {
        length += snprintf(message + length, sizeof(message) - length, "LY=%d ", state.LY);
        none = false;
    }
    if (state.RX) {
        length += snprintf(message + length, sizeof(message) - length, "RX=%d ", state.RX);
        none = false;
    }
    if (state.RY) {
        length += snprintf(message + length, sizeof(message) - length, "RY=%d ", state.RY);
        none = false;
    }
    if (state.DX) {
        length += snprintf(message + length, sizeof(message) - length, "DX=%d ", state.DX);
        none = false;
    }
    if (state.DY) {
        length += snprintf(message + length, sizeof(message) - length, "DY=%d ", state.DY);
        none = false;
    }
    if (state.LT) {
        length += snprintf(message + length, sizeof(message) - length, "LT=%d ", state.LT);
        none = false;
    }
    if (state.LB) {
        length += snprintf(message + length, sizeof(message) - length, "LB=%d ", state.LB);
        none = false;
    }
    if (state.RT) {
        length += snprintf(message + length, sizeof(message) - length, "RT=%d ", state.RT);
        none = false;
    }
    if (state.RB) {
        length += snprintf(message + length, sizeof(message) - length, "RB=%d ", state.RB);
        none = false;
    }
    if (state.L3) {
        length += snprintf(message + length, sizeof(message) - length, "L3=%d ", state.L3);
        none = false;
    }
    if (state.R3) {
        length += snprintf(message + length, sizeof(message) - length, "R3=%d ", state.R3);
        none = false;
    }
    if (state.L4) {
        length += snprintf(message + length, sizeof(message) - length, "L4=%d ", state.L4);
        none = false;
    }
    if (state.R4) {
        length += snprintf(message + length, sizeof(message) - length, "R4=%d ", state.R4);
        none = false;
    }
    if (state.LA) {
        length += snprintf(message + length, sizeof(message) - length, "LA=%d ", state.LA);
        none = false;
    }
    if (state.RA) {
        length += snprintf(message + length, sizeof(message) - length, "RA=%d ", state.RA);
        none = false;
    }
    if (state.A) {
        length += snprintf(message + length, sizeof(message) - length, "A=1 ");
        none = false;
    }
    if (state.B) {
        length += snprintf(message + length, sizeof(message) - length, "B=1 ");
        none = false;
    }
    if (state.X) {
        length += snprintf(message + length, sizeof(message) - length, "X=1 ");
        none = false;
    }
    if (state.Y) {
        length += snprintf(message + length, sizeof(message) - length, "Y=1 ");
        none = false;
    }
    if (state.start) {
        length += snprintf(message + length, sizeof(message) - length, "start=1 ");
        none = false;
    }
    if (state.back) {
        length += snprintf(message + length, sizeof(message) - length, "back=1 ");
        none = false;
    }
    if (state.mode) {
        length += snprintf(message + length, sizeof(message) - length, "mode=%d ", state.mode);
        none = false;
    }
    if (none) {
        length += snprintf(message + length, sizeof(message) - length, "none");
    }
    message[length] = '\0';
    print("%s\n", message);
}

void Controller::task() {}

void Controller::task(void* arg) {}

#endif
