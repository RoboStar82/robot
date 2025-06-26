
#include "ble.h"

#define bleDeviceName "RoboStar82"

BLEServer *bleServer = nullptr;

BLEUUID robotServiceUuid("b0b0c7ab-0000-4000-8000-000000000000");

BLEService *robotService = nullptr;

BLEServer *getBLEServer() {
    return bleServer;
}

BLEService *getRobotService() {
    return robotService;
}

void bleSetup() {
    BLEDevice::init(bleDeviceName);
    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(new ServerCallbacks);
    batterySetup(bleServer);
    robotService = bleServer->createService(robotServiceUuid);
    robotControlSetup(robotService);
    robotPositionSetup(robotService);
    robotHealthSetup(robotService);
    robotService->start();
    uartSetup(bleServer);
    BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
    bleAdvertising->addServiceUUID(robotServiceUuid);
    bleAdvertising->start();
}
