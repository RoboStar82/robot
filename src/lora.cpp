
#include "lora.h"

#include "controller.h"
#include "led.h"
#include "tea.h"

// https://github.com/jgromes/RadioLib
// https://github.com/ropg/heltec_esp32_lora_v3

Lora lora;

Lora::Lora() {
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
    module = new SX1262(new Module(SS, DIO1, RST_LoRa, BUSY_LoRa));
#endif
}

Lora::~Lora() {
#if ROBOT_HAS_CONTROLLER_LORA || ROBOT_HAS_TRANSCEIVER_LORA
    delete module;
#endif
}

void Lora::begin() {
    if (!module) {
        return;
    }
    module->begin(868.f, 125.f, 7, 5, 0xfa, 16);
#if ROBOT_HAS_TRANSCEIVER_LORA
    module->setPacketSentAction(packetSentCallback);
#endif
#if ROBOT_HAS_CONTROLLER_LORA
    module->setPacketReceivedAction(packetReceivedCallback);
    module->startReceive();
#endif
    if (!startedTask) {
        xTaskCreate(task, "lora_task", 4096, NULL, 1, &startedTask);
    }
}

void Lora::onPacketSent() {
    doneSend = true;
}

void Lora::onPacketReceived() {
    needRead = true;
}

void Lora::needSendControllerState() {
    needSend = true;
}

bool Lora::readControllerState() {
    int length = module->getPacketLength();
    if (length == 8) {
        uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        module->readData(data, length);
        log_d("%02x %02x %02x %02x %02x %02x %02x %02x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
        if (tea.decryptData(data)) {
            log_d("%02x %02x %02x %02x %02x %02x %02x %02x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
            controller.setState(data);
            return true;
        }
    }
    return false;
}

bool Lora::sendControllerState() {
    uint8_t data[8];
    controller.getState(data);
    tea.encryptData(data);
    return module->startTransmit(data, 8) == RADIOLIB_ERR_NONE;
}

void Lora::reset() {
    module->reset();
    begin();
}

void Lora::task() {
#if ROBOT_HAS_CONTROLLER_LORA
    while (true) {
        if (needRead) {
            needRead = false;
            if (readControllerState()) {
                errors = 0;
            } else {
                errors++;
                if (errors > 9) {
                    errors = 0;
                    reset();
                }
            }
        } else {
            vTaskDelay(1);
        }
    }
#endif
#if ROBOT_HAS_TRANSCEIVER_LORA
    while (true) {
        if (sending) {
            if (doneSend) {
                sending = false;
                doneSend = false;
                module->finishTransmit();
                led.setLoraSending(false);
                delays = 0;
                vTaskDelay(1);
            } else {
                vTaskDelay(1);
            }
        } else if (needSend) {
            needSend = false;
            if (sleeping) {
                sleeping = false;
                if (module->standby() != RADIOLIB_ERR_NONE) {
                    module->reset();
                    begin();
                }
                led.setLoraSleeping(false);
            }
            sending = true;
            led.setLoraSending(true);
            if (sendControllerState()) {
                errors = 0;
            } else {
                errors++;
                if (errors > 9) {
                    errors = 0;
                    reset();
                }
            }
            delays = 0;
            vTaskDelay(1);
        } else if (sleeping) {
            vTaskDelay(100);
        } else if (++delays >= 99999) {
            sleeping = true;
            module->sleep(true);
            led.setLoraSleeping(true);
            vTaskDelay(100);
        } else {
            vTaskDelay(1);
        }
    }
#endif
}

void Lora::packetSentCallback() {
    lora.onPacketSent();
}

void Lora::packetReceivedCallback() {
    lora.onPacketReceived();
}

void Lora::task(void* arg) {
    lora.task();
}
