
#include "led.h"

Led led;

Led::Led() {}

Led::~Led() {}

void Led::begin() {
    setPowerOn(true);
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
    if (timers.power.on.value) {
        r = g = b = 0x33;
    } else if (timers.ota.ble.value) {
        g = b = 0x11;
    } else if (timers.ota.wifi.value) {
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
    if (timers.power.on.value) {
        c = 0x11;
    } else if (timers.lora.errors.value) {
        c = 0x01;
    } else if (timers.ota.ble.value) {
        c = 0x01;
    } else if (timers.ota.wifi.value) {
        c = 0x01;
    }
    if (state.lora.sending) {
        c++;
    } else if (state.lora.sleeping) {
        c = timers.lora.sleeping.value ? 1 : 0;
    }
    ledcWrite(ledPin, c);
#endif
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
        timers.ota.ble.counter = 0;
        timers.ota.ble.value = false;
        timers.ota.wifi.counter = 0;
        timers.ota.wifi.value = false;
        state.lora.sleeping = value;
        changed = true;
    }
}

void Led::setOtaBLE(bool value) {
    if (state.ota.ble != value) {
        timers.ota.ble.counter = 0;
        timers.ota.ble.value = false;
        timers.ota.wifi.counter = 0;
        timers.ota.wifi.value = false;
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
        vTaskDelay(10);
    }
}

void Led::task(void* arg) {
    led.task();
}
