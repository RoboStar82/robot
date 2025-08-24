
#pragma once

#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME "RoboStar82"
#endif

#include <NimBLEDevice.h>

#include "battery.h"
#include "controller.h"
#include "health.h"
#include "position.h"
#include "print.h"
#include "settings.h"
#include "uart.h"

class ServerCallbacks : public BLEServerCallbacks {
   public:
    void onConnect(BLEServer *bleServer, BLEConnInfo &connInfo);

    void onDisconnect(BLEServer *bleServer, BLEConnInfo &connInfo, int reason);
};

BLEServer *getBLEServer();

BLEService *getRobotService();

void bleSetup();
