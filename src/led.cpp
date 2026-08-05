
#include "config.h"

#ifdef ROBOT_HAS_LED

#include "led.h"

Led led;

Led::Led() {}

Led::~Led() {}

void Led::begin() {
    setPower(true);
#ifdef RGB_BUILTIN
    rgbPin = RGB_BUILTIN;
    rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
#else
#ifdef LED_BUILTIN
    ledPin = LED_BUILTIN;
    ledcAttach(LED_BUILTIN, 5000, 8);
#endif
#endif
    xTaskCreate(task, "led_task", 4096, NULL, 1, NULL);
}

void Led::onChange() {
#ifdef RGB_BUILTIN
    uint8_t r = 0x00, g = 0x00, b = 0x00;
    if (timers.power.state.value) {
        r = g = b = 0x33;
    } else if (timers.ble.state.value) {
        g = b = 0x11;
    } else if (timers.wifi.state.value) {
        g = b = 0x11;
    } else if (state.controller.button == 'A') {
        g = 0x11;
    } else if (state.controller.button == 'B') {
        r = 0x11;
    } else if (state.controller.button == 'X') {
        b = 0x11;
    } else if (state.controller.button == 'Y') {
        r = g = 0x11;
    }
    rgbLedWrite(rgbPin, r, g, b);
#else
#ifdef LED_BUILTIN
    uint8_t c = 0x00;
    if (timers.power.state.value) {
        c = 0x11;
    } else if (timers.ble.state.value) {
        c = 0x01;
    } else if (timers.wifi.state.value) {
        c = 0x01;
    } else if (timers.lora.errors.value) {
        c = 0x01;
    }
    if (state.lora.sending) {
        c++;
    } else if (state.lora.sleeping) {
        c = timers.lora.sleeping.value ? 1 : 0;
    }
    ledcWrite(ledPin, 0 < c && c < 0x11 ? 0 : 0xff);
#endif
#endif
}

void Led::setPower(bool value) {
    if (state.power.value != value) {
        timers.power.state.counter = 0;
        timers.power.state.value = false;
        state.power.value = value;
        changed = true;
    }
}

void Led::setBLE(bool value) {
    if (state.ble.value != value) {
        timers.ble.state.counter = 0;
        timers.ble.state.value = false;
        timers.wifi.state.counter = 0;
        timers.wifi.state.value = false;
        state.ble.value = value;
        changed = true;
    }
}

void Led::setWiFi(bool value) {
    if (state.wifi.value != value) {
        timers.wifi.state.counter = 0;
        timers.wifi.state.value = false;
        timers.ble.state.counter = 0;
        timers.ble.state.value = false;
        state.wifi.value = value;
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
        timers.ble.state.counter = 0;
        timers.ble.state.value = false;
        timers.wifi.state.counter = 0;
        timers.wifi.state.value = false;
        state.lora.sleeping = value;
        changed = true;
    }
}

void Led::setControllerButton(char value) {
    if (state.controller.button != value) {
        state.controller.button = value;
        changed = true;
    }
}

void Led::task() {
    while (true) {
        if (state.power.value) {
            switch (++timers.power.state.counter % timers.power.state.period) {
                case 1:
                    timers.power.state.value = true;
                    changed = true;
                    break;
                case 0:
                    timers.power.state.value = false;
                    state.power.value = false;
                    changed = true;
                    break;
            }
        }
        if (state.ble.value) {
            switch (++timers.ble.state.counter % timers.ble.state.period) {
                case 1:
                    timers.ble.state.value = true;
                    changed = true;
                    break;
                case 11:
                    timers.ble.state.value = false;
                    changed = true;
                    break;
            }
        }
        if (state.wifi.value) {
            switch (++timers.wifi.state.counter % timers.wifi.state.period) {
                case 21:
                case 41:
                    timers.wifi.state.value = true;
                    changed = true;
                    break;
                case 31:
                case 51:
                    timers.wifi.state.value = false;
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

#endif
