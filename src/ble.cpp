
#include "ble.h"

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
    BLEDevice::init("");
    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(new ServerCallbacks);
    BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
    bleAdvertising->setName(BLE_DEVICE_NAME);
#if ROBOT_HAS_BATTERY
    batterySetup(bleServer);
#endif
    robotService = bleServer->createService(robotServiceUuid);
#if ROBOT_HAS_CONTROLLER_BLE
    settingsSetup(robotService);
    controllerSetup(robotService);
#endif
#if ROBOT_HAS_LIDAR
    positionSetup(robotService);
#endif
    healthSetup(robotService);
    robotService->start();
    uartSetup(bleServer);
    bleAdvertising->addServiceUUID(robotServiceUuid);
    bleAdvertising->start();
}
