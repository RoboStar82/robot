
#pragma once

// https://github.com/jgromes/RadioLib
// https://github.com/ropg/heltec_esp32_lora_v3

#include <RadioLib.h>

#if ROBOT_HAS_CONTROLLER_LORA
#define DIO1 14
#define RST_LoRa 3
#define BUSY_LoRa 46
#endif

class Lora {
   public:
    Lora();

    void begin();

    void onPacketSent();
    void onPacketReceived();

    void needSendControllerState();

    void reset();
    void task();

   protected:
    SX1262* module = nullptr;

    int errors = 0;
    int delays = 0;
    bool sending = false;
    bool sleeping = false;

    bool needRead = false;
    bool needSend = false;
    bool doneSend = false;

    TaskHandle_t* taskHandle = nullptr;

    bool readControllerState();
    bool sendControllerState();

    static void packetSentCallback();
    static void packetReceivedCallback();

    static void task(void* arg);
};

extern Lora lora;
