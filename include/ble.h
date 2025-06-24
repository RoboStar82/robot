
#pragma once

#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLE2902.h>

#include "battery.h"

#define BLE_DEVICE_NAME "RoboStar82"

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* bleServer);
    void onDisconnect(BLEServer* bleServer);
};

class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *bleCharacteristic);
    void onRobotControl(std::string value);
};

BLEServer *getBLEServer();

BLEService *getRobotService();

BLEService *getUARTService();

BLECharacteristic *getRobotControlCharacteristic();

BLECharacteristic *getRobotPositionCharacteristic();

BLECharacteristic *getRobotHealthCharacteristic();

BLECharacteristic *getUARTRxCharacteristic();

BLECharacteristic *getUARTTxCharacteristic();

void bleSetup();
