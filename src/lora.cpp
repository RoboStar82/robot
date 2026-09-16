
#include <Arduino.h>
#include <FreeRTOS.h>
#include <RadioLib.h>
#include <SPI.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_LORA

#include "lora.h"

#ifdef ROBOT_HAS_CONTROLLER
#include "controller.h"
#endif

#ifdef ROBOT_HAS_LED
#include "led.h"
#endif

#ifdef ROBOT_HAS_TEA
#include "tea.h"
#endif

#include "print.h"

Lora lora;

Lora::Lora() {}

void Lora::begin() {
#ifdef ARDUINO_ARCH_STM32
    spi.begin();
#endif
    int16_t r;
    r = radio.beginFSK(configFSK);
    print("[LoRa] beginFSK(): 0x%02x\n", r);
    return;
#ifdef ROBOT_HAS_CONTROLLER_LORA
    radio.setPacketReceivedAction(packetReceivedCallback);
    r = radio.startReceive();
    print("[LoRa] startReceive(): 0x%02x\n", r);
#endif
#ifdef ROBOT_HAS_PROXY_LORA
    radio.setPacketSentAction(packetSentCallback);
#endif
    if (!taskHandle) {
        xTaskCreate(task, "lora_task", 4096, NULL, 1, &taskHandle);
    }
}

void Lora::end() {
    if (taskHandle) {
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
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
    size_t length = radio.getPacketLength();
    print("[LoRa] packet length: %d\n", length);
    if (length == 8) {
        uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        radio.readData(data, length);
        print("[LoRa] packet: 0x%02x%02x%02x%02x%02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
#ifdef ROBOT_HAS_TEA
        if (tea.decryptData(data)) {
            log_d("%02x %02x %02x %02x %02x %02x %02x %02x", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
            controller.setState(data);
            return true;
        }
#endif
    }
    return false;
}

bool Lora::sendControllerState() {
    /*
    uint8_t data[8];
    controller.getState(data);
    tea.encryptData(data);
    return radio.startTransmit(data, 8) == RADIOLIB_ERR_NONE;
    */
    return false;
}

void Lora::reset() {
    radio.reset();
    begin();
}

void Lora::task() {
#ifdef ROBOT_HAS_CONTROLLER_LORA
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
#if ROBOT_HAS_PROXY_LORA
    while (true) {
        if (sending) {
            if (doneSend) {
                sending = false;
                doneSend = false;
                radio.finishTransmit();
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
                if (radio.standby() != RADIOLIB_ERR_NONE) {
                    radio.reset();
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
            radio.sleep(true);
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

#endif
