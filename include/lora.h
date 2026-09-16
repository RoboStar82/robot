
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <RadioLib.h>
#include <SPI.h>
#include <task.h>

#include "config.h"

#include "delay.h"

class LoraArduinoHal : public ArduinoHal {
   public:
    LoraArduinoHal(SPIClass& spi, SPISettings spiSettings) : ArduinoHal(spi, spiSettings) {}
    inline void delay(RadioLibTime_t ms) override {
        vTaskDelayMS(ms);
    }
};

class Lora {
   public:
    Lora();
    virtual ~Lora() = default;

    void begin();
    void end();

    void onPacketSent();
    void onPacketReceived();

    void needSendControllerState();

    void reset();

    void task();

   protected:
    TaskHandle_t taskHandle = nullptr;

#ifdef ARDUINO_ARCH_ESP32
    Module module = Module(LORA_NSS_PIN, LORA_DIO1_PIN, LORA_NRST_PIN, LORA_BUSY_PIN);
    SX1262 radio = SX1262(&module);
#endif

#ifdef ARDUINO_ARCH_STM32
    LoraArduinoHal hal = LoraArduinoHal(spi, spiSettings);
    SPIClass spi = SPIClass(LORA_MOSI_PIN, LORA_MISO_PIN, LORA_SCK_PIN);
    SPISettings spiSettings = SPISettings(2000000, MSBFIRST, SPI_MODE0);
    Module module = Module(&hal, LORA_NSS_PIN, LORA_DIO1_PIN, LORA_NRST_PIN, LORA_BUSY_PIN);
    SX1262 radio = SX1262(&module);
#endif

    ConfigFSK_t configFSK = {
        .frequency = 868.f,
        .bitRate = 300.f,
        .frequencyDeviation = 150.f,
        .receiverBandwidth = 250.f,
        .power = 22,
        .preambleLength = 16,
    };

    int errors = 0;
    int delays = 0;
    bool sending = false;
    bool sleeping = false;

    bool needRead = false;
    bool needSend = false;
    bool doneSend = false;

    bool readControllerState();
    bool sendControllerState();

    static inline void packetSentCallback();
    static inline void packetReceivedCallback();

    static inline void task(void* arg);
};

extern Lora lora;
