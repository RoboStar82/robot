
#include "controller.h"

#include "led.h"
#include "lora.h"
#include "ota.h"
#include "robot.h"

Controller controller;

Controller::Controller() {}

Controller::~Controller() {}

void Controller::begin() {
#if ROBOT_HAS_CONTROLLER_SERIAL
    ControllerSerial.begin(115200, SERIAL_8N1, ROBOT_CONTROLLER_SERIAL_RX_PIN, ROBOT_CONTROLLER_SERIAL_TX_PIN);
    xTaskCreate(task, "controller_task", 4096, NULL, 1, NULL);
#endif
#if ROBOT_HAS_TRANSCEIVER_SERIAL
    ControllerSerial.begin(115200, SERIAL_8N1, ROBOT_TRANSCEIVER_SERIAL_RX_PIN, ROBOT_TRANSCEIVER_SERIAL_TX_PIN);
    xTaskCreate(task, "controller_task", 4096, NULL, 1, NULL);
#endif
}

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
#if ROBOT_HAS_AUTO_MODE
    if (state.back) {
        robot.needUpdateAutoStop();
    }
    if (!state.start && oldState.start) {
        robot.needUpdateAutoStart();
    }
#endif
#if ROBOT_HAS_CHASSIS
    if (state.lx != oldState.lx || state.ly != oldState.ly || state.rx != oldState.rx || state.ry != oldState.ry || state.dx != oldState.dx) {
        robot.needUpdateSpeed();
    }
    if (state.dy != oldState.dy || state.a != oldState.a || state.b != oldState.b || state.x != oldState.x || state.y != oldState.y) {
        robot.needUpdateServo();
    }
#endif
#if ROBOT_HAS_TRANSCEIVER_LORA
    lora.needSendControllerState();
#endif
#if ROBOT_HAS_TRANSCEIVER_SERIAL
    needSendState();
#endif
#ifdef RGB_BUILTIN
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
                ota.needEnableBLE();
            }
            if (state.y) {
                ota.needEnableWiFi();
            }
        }
        if (state.x) {
            if (state.b) {
                ota.needDisableBLE();
            }
            if (state.y) {
                ota.needDisableWiFi();
            }
        }
    }
    print();
}

void Controller::print() {
    size_t length = 0;
    char message[256] = {0};
    length += snprintf(message + length, 256 - length, "[%6u][I] Controller: mode=%x ", (unsigned long)(esp_timer_get_time() / 1000ULL), state.mode);
    bool none = true;
    if (state.lx) {
        length += snprintf(message + length, 256 - length, "lx=%d ", state.lx);
        none = false;
    }
    if (state.ly) {
        length += snprintf(message + length, 256 - length, "ly=%d ", state.ly);
        none = false;
    }
    if (state.rx) {
        length += snprintf(message + length, 256 - length, "rx=%d ", state.rx);
        none = false;
    }
    if (state.ry) {
        length += snprintf(message + length, 256 - length, "ry=%d ", state.ry);
        none = false;
    }
    if (state.dx) {
        length += snprintf(message + length, 256 - length, "dx=%d ", state.dx);
        none = false;
    }
    if (state.dy) {
        length += snprintf(message + length, 256 - length, "dy=%d ", state.dy);
        none = false;
    }
    if (state.lz) {
        length += snprintf(message + length, 256 - length, "lz=%d ", state.lz);
        none = false;
    }
    if (state.rz) {
        length += snprintf(message + length, 256 - length, "rz=%d ", state.rz);
        none = false;
    }
    if (state.lt) {
        length += snprintf(message + length, 256 - length, "lt=%d ", state.lt);
        none = false;
    }
    if (state.rt) {
        length += snprintf(message + length, 256 - length, "rt=%d ", state.rt);
        none = false;
    }
    if (state.a) {
        length += snprintf(message + length, 256 - length, "a=1 ");
        none = false;
    }
    if (state.b) {
        length += snprintf(message + length, 256 - length, "b=1 ");
        none = false;
    }
    if (state.x) {
        length += snprintf(message + length, 256 - length, "x=1 ");
        none = false;
    }
    if (state.y) {
        length += snprintf(message + length, 256 - length, "y=1 ");
        none = false;
    }
    if (state.start) {
        length += snprintf(message + length, 256 - length, "start=1 ");
        none = false;
    }
    if (state.back) {
        length += snprintf(message + length, 256 - length, "back=1 ");
        none = false;
    }
    if (none) {
        length += snprintf(message + length, 256 - length, "none");
    }
    message[length] = '\n';
    length++;
    message[length] = '\0';
    log_printf(message);
}

void Controller::needSendState() {
    bool value = true;
    xQueueSend(needQueue, &value, 0);
}

void Controller::sendState() {
#if ROBOT_HAS_TRANSCEIVER_SERIAL
    char message[32] = {0};
    uint8_t* data = (uint8_t*)(&state);
    int length = snprintf(message, 32, "CTRL=%02x%02x%02x%02x%02x%02x%02x%02x\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    ControllerSerial.write(message, length);
#endif
}

void Controller::task() {
#if ROBOT_HAS_CONTROLLER_SERIAL
    while (true) {
        if (ControllerSerial.available()) {
            uint8_t data[32];
            size_t length = ControllerSerial.readBytesUntil('\n', data, 32);
            if (length > 20 && strncmp((const char*)data, "CTRL=", 5) == 0) {
                data[0] = ((data[5] <= '9' ? data[5] - '0' : toupper(data[5]) - 'A' + 10) << 4) | (data[6] <= '9' ? data[6] - '0' : toupper(data[6]) - 'A' + 10);
                data[1] = ((data[7] <= '9' ? data[7] - '0' : toupper(data[7]) - 'A' + 10) << 4) | (data[8] <= '9' ? data[8] - '0' : toupper(data[8]) - 'A' + 10);
                data[2] = ((data[9] <= '9' ? data[9] - '0' : toupper(data[9]) - 'A' + 10) << 4) | (data[10] <= '9' ? data[10] - '0' : toupper(data[10]) - 'A' + 10);
                data[3] = ((data[11] <= '9' ? data[11] - '0' : toupper(data[11]) - 'A' + 10) << 4) | (data[12] <= '9' ? data[12] - '0' : toupper(data[12]) - 'A' + 10);
                data[4] = ((data[13] <= '9' ? data[13] - '0' : toupper(data[13]) - 'A' + 10) << 4) | (data[14] <= '9' ? data[14] - '0' : toupper(data[14]) - 'A' + 10);
                data[5] = ((data[15] <= '9' ? data[15] - '0' : toupper(data[15]) - 'A' + 10) << 4) | (data[16] <= '9' ? data[16] - '0' : toupper(data[16]) - 'A' + 10);
                data[6] = ((data[17] <= '9' ? data[17] - '0' : toupper(data[17]) - 'A' + 10) << 4) | (data[18] <= '9' ? data[18] - '0' : toupper(data[18]) - 'A' + 10);
                data[7] = ((data[19] <= '9' ? data[19] - '0' : toupper(data[19]) - 'A' + 10) << 4) | (data[20] <= '9' ? data[20] - '0' : toupper(data[20]) - 'A' + 10);
                setState(data);
            }
        } else {
            vTaskDelay(1);
        }
    }
#endif
#if ROBOT_HAS_TRANSCEIVER_SERIAL
    while (true) {
        bool update;
        if (xQueueReceive(needQueue, &update, 1000)) {
            sendState();
        }
    }
#endif
}

void Controller::task(void* arg) {
    controller.task();
}
