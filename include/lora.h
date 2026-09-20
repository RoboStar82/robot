
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

    inline void delayMicroseconds(RadioLibTime_t us) override {
        vTaskDelayMicroseconds(us);
    }
};

class Lora {
   public:
    Lora();
    virtual ~Lora() = default;

    void begin();
    void end();

    void init();
    void reset();
    void sleep();
    void wakeup();

    void onPacketSent();
    void onPacketReceived();

    void onControllerChange();

    void task();

   protected:
    TaskHandle_t taskHandle = nullptr;
    QueueHandle_t taskQueue = xQueueCreate(4, sizeof(int));

#ifdef ARDUINO_ARCH_ESP32
    Module module = Module(LORA_NSS_PIN, LORA_DIO1_PIN, LORA_NRST_PIN, LORA_BUSY_PIN);
    SX1262 radio = SX1262(&module);
#endif

#ifdef ARDUINO_ARCH_STM32
    SPIClass spi = SPIClass(LORA_MOSI_PIN, LORA_MISO_PIN, LORA_SCK_PIN);
    SPISettings spiSettings = SPISettings(2000000, MSBFIRST, SPI_MODE0);
    LoraArduinoHal hal = LoraArduinoHal(spi, spiSettings);
    Module module = Module(&hal, LORA_NSS_PIN, LORA_DIO1_PIN, LORA_NRST_PIN, LORA_BUSY_PIN);
    SX1262 radio = SX1262(&module);
#endif

    ConfigLoRa_t configLoRa = {
        .frequency = 868.0,
        .bandwidth = 125.0,
        .spreadingFactor = 7,
        .codingRate = 5,
#ifdef LORA_SYNC_WORD
        .syncWord = (uint8_t)LORA_SYNC_WORD[0],
#endif
        .power = 16,
        .preambleLength = 16,
    };

    ConfigFSK_t configFSK = {
        .frequency = 868.0,
        .bitRate = 4.8,
        .frequencyDeviation = 5.0,
        .receiverBandwidth = 125.0,
        .power = 16,
        .preambleLength = 16,
    };

    int errorCount = 0;

    bool isSleeping = false;

    bool readControllerState();
    bool sendControllerState();

    static inline void packetSentCallback();
    static inline void packetReceivedCallback();

    static inline void task(void* arg);
};

extern Lora lora;
