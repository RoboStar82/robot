
#include <Arduino.h>
#include <FreeRTOS.h>
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

void Lora::init() {
    int e;
#ifdef ROBOT_HAS_LORA_FSK
    e = radio.beginFSK(configFSK);
    print("[LoRa] beginFSK(): 0x%02x\n", e);
#ifdef LORA_SYNC_WORD
    e = radio.setSyncWord((uint8_t*)LORA_SYNC_WORD, strlen(LORA_SYNC_WORD));
    print("[LoRa] setSyncWord(): 0x%02x\n", e);
#endif
#else
    e = radio.begin(configLoRa);
    print("[LoRa] begin(): 0x%02x\n", e);
#endif
#ifdef ROBOT_HAS_CONTROLLER_LORA
    radio.setPacketReceivedAction(packetReceivedCallback);
    e = radio.startReceive();
    print("[LoRa] startReceive(): 0x%02x\n", e);
#endif
#ifdef ROBOT_HAS_PROXY_LORA
    radio.setPacketSentAction(packetSentCallback);
#endif
}

void Lora::reset() {
    int e;
    e = radio.reset();
    print("[LoRa] reset(): 0x%02x\n", e);
    init();
}

void Lora::sleep() {
    int e;
    e = radio.sleep(true);
    print("[LoRa] sleep(): 0x%02x\n", e);
    if (e == RADIOLIB_ERR_NONE) {
        isSleeping = true;
#ifdef ROBOT_HAS_LED
        led.setLoraSleeping(true);
#endif
    }
}

void Lora::wakeup() {
    int e;
    e = radio.standby();
    print("[LoRa] wakeup(): 0x%02x\n", e);
    if (e != RADIOLIB_ERR_NONE) {
        reset();
    }
    isSleeping = false;
#ifdef ROBOT_HAS_LED
    led.setLoraSleeping(false);
#endif
}

void Lora::onPacketSent() {
    if (taskHandle) {
        xTaskNotifyFromISR(taskHandle, 1, eSetBits, NULL);
    }
}

void Lora::onPacketReceived() {
    if (taskHandle) {
        xTaskNotifyFromISR(taskHandle, 2, eSetBits, NULL);
    }
}

void Lora::onControllerChange() {
    int value = 0;
    xQueueSend(taskQueue, &value, 0);
}

bool Lora::readControllerState() {
    size_t length = radio.getPacketLength();
    print("[LoRa] packet length: %d\n", length);
    if (length == CONTROLLER_STATE_SIZE) {
        uint8_t data[CONTROLLER_STATE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
        if (radio.readData(data, length) == RADIOLIB_ERR_NONE) {
            print("[LoRa] packet: 0x%02x%02x%02x%02x%02x%02x%02x%02x\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
#ifdef ROBOT_HAS_CONTROLLER
            controller.onInput(data, length);
#endif
            return true;
        }
    }
    return false;
}

bool Lora::sendControllerState() {
    bool r = false;
#ifdef ROBOT_HAS_CONTROLLER
    uint8_t data[CONTROLLER_STATE_SIZE];
    controller.getState(data);
    int e = radio.startTransmit(data, CONTROLLER_STATE_SIZE);
    print("[LoRa] startTransmit(): 0x%02x\n", e);
    if (e == RADIOLIB_ERR_NONE) {
#ifdef ROBOT_HAS_LED
        led.setLoraSending(true);
#endif
        r = ulTaskNotifyTakeMS(true, 1000) == pdTRUE;
        e = radio.finishTransmit();
        print("[LoRa] finishTransmit(): 0x%02x\n", e);
#ifdef ROBOT_HAS_LED
        led.setLoraSending(false);
#endif
    }
#endif
    return r;
}

void Lora::task() {
    init();
    while (true) {
#ifdef ROBOT_HAS_CONTROLLER_LORA
        if (ulTaskNotifyTakeMS(true, 1000)) {
            if (readControllerState()) {
                errorCount = 0;
            } else {
                errorCount++;
                if (errorCount > 9) {
                    errorCount = 0;
                    reset();
                }
            }
        }
#else
#ifdef ROBOT_HAS_PROXY_LORA
        int value;
        if (xQueueReceiveMS(taskQueue, &value, 99999) == pdTRUE) {
            if (isSleeping) {
                wakeup();
            }
            if (sendControllerState()) {
                errorCount = 0;
            } else {
                errorCount++;
                if (errorCount > 9) {
                    errorCount = 0;
                    reset();
                }
            }
        } else {
            if (!isSleeping) {
                sleep();
            }
        }
#else
        vTaskDelayMS(9999);
        if (!isSleeping) {
            sleep();
        }
#endif
#endif
    }
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
