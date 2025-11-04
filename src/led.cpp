
#include "led.h"

Led led;

Led::Led() {
#if ROBOT_HAS_RGB_LED
    neoPixel = new Adafruit_NeoPixel(1, 48, NEO_GRB + NEO_KHZ800);
#endif
}

void Led::begin() {
#if ROBOT_HAS_TRANSCEIVER_LORA
    ledcAttach(LED_BUILTIN, 5000, 8);
#endif
    if (neoPixel) {
        neoPixel->begin();
        neoPixel->setPixelColor(0, 0x000000);
        neoPixel->show();
    }
    setPowerOn(true);
    xTaskCreate(task, "led_task", 4096, NULL, 1, NULL);
}

void Led::onChange() {
#if ROBOT_HAS_RGB_LED
    if (state.controller.button == '\0') {
        neoPixel->setPixelColor(0, 0x000000);
    } else if (state.controller.button == 'A') {
        neoPixel->setPixelColor(0, 0x001100);
    } else if (state.controller.button == 'B') {
        neoPixel->setPixelColor(0, 0x110000);
    } else if (state.controller.button == 'X') {
        neoPixel->setPixelColor(0, 0x000011);
    } else if (state.controller.button == 'Y') {
        neoPixel->setPixelColor(0, 0x111100);
    } else {
        neoPixel->setPixelColor(0, 0x000000);
    }
    neoPixel->show();
#endif
#if ROBOT_HAS_TRANSCEIVER_LORA
    int value = 0;
    if (timers.power.on.value) {
        value += 9;
    }
    if (timers.lora.sleeping.value) {
        value++;
    }
    if (timers.lora.errors.value) {
        value++;
    }
    if (state.lora.sending) {
        value++;
    }
    if (timers.ota.ble.value) {
        value++;
    }
    if (timers.ota.wifi.value) {
        value++;
    }
    ledcWrite(LED_BUILTIN, value);
#endif
}

void Led::setPowerOn(bool value) {
    if (state.power.on != value) {
        timers.power.on.counter = 0;
        timers.power.on.value = false;
        state.power.on = value;
        changed = true;
    }
}

void Led::setControllerButton(char value) {
    if (state.controller.button != value) {
        state.controller.button = value;
        changed = true;
    }
}

void Led::setLoraErrors(bool value) {
    if (state.lora.errors != value) {
        timers.lora.sleeping.counter = 0;
        timers.lora.sleeping.value = false;
        state.lora.errors = value;
        changed = true;
    }
}

void Led::setLoraSending(bool value) {
    if (state.lora.sending != value) {
        timers.lora.errors.counter = 0;
        timers.lora.errors.value = false;
        timers.lora.sleeping.counter = 0;
        timers.lora.sleeping.value = false;
        state.lora.sending = value;
        changed = true;
    }
}

void Led::setLoraSleeping(bool value) {
    if (state.lora.sleeping != value) {
        timers.lora.errors.counter = 0;
        timers.lora.errors.value = false;
        state.lora.sleeping = value;
        changed = true;
    }
}

void Led::setOtaBle(bool value) {
    if (state.ota.ble != value) {
        timers.ota.ble.counter = 0;
        timers.ota.ble.value = false;
        timers.ota.wifi.counter = 0;
        timers.ota.wifi.value = false;;
        state.ota.ble = value;
        changed = true;
    }
}

void Led::setOtaWiFi(bool value) {
    if (state.ota.wifi != value) {
        timers.ota.wifi.counter = 0;
        timers.ota.wifi.value = false;
        timers.ota.ble.counter = 0;
        timers.ota.ble.value = false;
        state.ota.wifi = value;
        changed = true;
    }
}

void Led::task() {
    while (true) {
        if (state.power.on) {
            switch (++timers.power.on.counter % timers.power.on.period) {
                case 1:
                    timers.power.on.value = true;
                    changed = true;
                    break;
                case 0:
                    timers.power.on.value = false;
                    state.power.on = false;
                    changed = true;
                    break;
            }
        }
        if (state.lora.errors) {
            switch (++timers.lora.errors.counter % timers.lora.errors.period) {
                case 0:
                    timers.lora.errors.value = !timers.lora.errors.value;
                    changed = true;
                    break;
            }
        }
        if (state.lora.sleeping) {
            switch (++timers.lora.sleeping.counter % timers.lora.sleeping.period) {
                case 0:
                    timers.lora.sleeping.value = !timers.lora.sleeping.value;
                    changed = true;
                    break;
            }
        }
        if (state.ota.ble) {
            switch (++timers.ota.ble.counter % timers.ota.ble.period) {
                case 1:
                    timers.ota.ble.value = true;
                    changed = true;
                    break;
                case 11:
                    timers.ota.ble.value = false;
                    changed = true;
                    break;
            }
        }
        if (state.ota.wifi) {
            switch (++timers.ota.wifi.counter % timers.ota.wifi.period) {
                case 21:
                case 41:
                    timers.ota.wifi.value = true;
                    changed = true;
                    break;
                case 31:
                case 51:
                    timers.ota.wifi.value = false;
                    changed = true;
                    break;
            }
        }
        if (changed) {
            changed = false;
            onChange();
        }
        counter++;
        delay(10);
    }
}

void Led::task(void* arg) {
    led.task();
}
