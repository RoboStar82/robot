
#include "config.h"

#ifdef ROBOT_HAS_CONTROLLER

#include "controller.h"
#include "print.h"

Controller controller;

Controller::Controller() {}

Controller::~Controller() {}

void Controller::begin() {}

void Controller::end() {}

void Controller::onRawInput(uint8_t* data, size_t length, uint16_t vendorId, uint16_t productId) {
    if (rawInput.vendorId == vendorId && rawInput.productId == productId && rawInput.length == length) {
        if (memcmp(rawInput.data, data, length) == 0) {
            return;
        }
    }
    rawInput.vendorId = vendorId;
    rawInput.productId = productId;
    rawInput.length = length;
    memcpy(rawInput.data, data, length);
    print("[Controller] input %04x:%04x %d: 0x", vendorId, productId, length);
    for (int i = 0; i < length; i++) {
        print("%02x", data[i]);
    }
    print("\n");
}

void Controller::task() {}

void Controller::task(void* arg) {}

#endif
