
#include "controller.h"

#include "led.h"
#include "lora.h"
#include "ota.h"
#include "robot.h"

Controller controller;

controller_state_t Controller::getState() {
    return state;
}

void Controller::getState(uint8_t copyState[]) {
    memcpy(copyState, &state, sizeof(controller_state_t));
}

void Controller::getState(controller_state_t* copyState) {
    memcpy(copyState, &state, sizeof(controller_state_t));
}

void Controller::setState(controller_state_t newState) {
    if (memcmp(&newState, &state, sizeof(controller_state_t))) {
        controller_state_t oldState = state;
        state = newState;
        onChange(oldState);
    }
}

void Controller::setState(controller_state_t* newState) {
    setState(*newState);
}

void Controller::setState(uint8_t newState[]) {
    setState(*((controller_state_t*)newState));
}

void Controller::onChange(controller_state_t oldState) {
#if ROBOT_HAS_TRANSCEIVER_LORA
    lora.needSendControllerState();
#endif
#if ROBOT_HAS_WHEELS
    robot.updateSpeed();
#endif
#if ROBOT_HAS_RGB_LED
    if (false) {
    } else if (state.a && !oldState.a) {
        led.setControllerButton('A');
    } else if (state.b && !oldState.b) {
        led.setControllerButton('B');
    } else if (state.x && !oldState.x) {
        led.setControllerButton('X');
    } else if (state.y && !oldState.y) {
        led.setControllerButton('Y');
    } else {
        led.setControllerButton(0);
    }
#endif
    if (state.back) {
        if (state.a) {
            if (state.b) {
                ota.enableBLE();
            }
            if (state.y) {
                ota.enableWiFi();
            }
        }
        if (state.x) {
            if (state.b) {
                ota.disableBLE();
            }
            if (state.y) {
                ota.disableWiFi();
            }
        }
    }
    print();
}

void Controller::print() {
    Serial.printf("[%6u][I] Controller: mode=%x ", (unsigned long)(esp_timer_get_time() / 1000ULL), state.mode);
    bool none = true;
    if (state.lx) {
        Serial.printf("lx=%d ", state.lx);
        none = false;
    }
    if (state.ly) {
        Serial.printf("ly=%d ", state.ly);
        none = false;
    }
    if (state.rx) {
        Serial.printf("rx=%d ", state.rx);
        none = false;
    }
    if (state.ry) {
        Serial.printf("ry=%d ", state.ry);
        none = false;
    }
    if (state.dx) {
        Serial.printf("dx=%d ", state.dx);
        none = false;
    }
    if (state.dy) {
        Serial.printf("dy=%d ", state.dy);
        none = false;
    }
    if (state.lz) {
        Serial.printf("lz=%d ", state.lz);
        none = false;
    }
    if (state.rz) {
        Serial.printf("rz=%d ", state.rz);
        none = false;
    }
    if (state.lt) {
        Serial.printf("lt=%d ", state.lt);
        none = false;
    }
    if (state.rt) {
        Serial.printf("rt=%d ", state.rt);
        none = false;
    }
    if (state.a) {
        Serial.printf("a=1 ");
        none = false;
    }
    if (state.b) {
        Serial.printf("b=1 ");
        none = false;
    }
    if (state.x) {
        Serial.printf("x=1 ");
        none = false;
    }
    if (state.y) {
        Serial.printf("y=1 ");
        none = false;
    }
    if (state.start) {
        Serial.printf("start=1 ");
        none = false;
    }
    if (state.back) {
        Serial.printf("back=1 ");
        none = false;
    }
    if (none) {
        Serial.print("none");
    }
    Serial.print("\n");
}
