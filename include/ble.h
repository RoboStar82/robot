
#pragma once

#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME "RoboStar82"
#endif

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include "battery.h"
#include "controller.h"
#include "health.h"
#include "position.h"
#include "uart.h"
#include "print.h"

class ServerCallbacks : public BLEServerCallbacks {
   public:
    void onConnect(BLEServer *bleServer) {
        println("V: BLE: connected");
    }

    void onDisconnect(BLEServer *bleServer) {
        println("V: BLE: disconnected");
        BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
        bleAdvertising->start();
    }
};

BLEServer *getBLEServer();

BLEService *getRobotService();

void bleSetup();
