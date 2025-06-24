
#include "ble.h"

BLEServer *bleServer;

BLEUUID robotServiceUuid("b0b0c7ab-0000-4000-8000-000000000000");
BLEService *robotService;

BLE2902 *robotControl2902;
BLEDescriptor *robotControl2901;
BLEUUID robotControlCharacteristicUuid("b0b0c7ab-0001-4000-8000-000000000000");
BLECharacteristic *robotControlCharacteristic;

BLE2902 *robotPosition2902;
BLEDescriptor *robotPosition2901;
BLEUUID robotPositionCharacteristicUuid("b0b0c7ab-0002-4000-8000-000000000000");
BLECharacteristic *robotPositionCharacteristic;

BLE2902 *robotHealth2902;
BLEDescriptor *robotHealth2901;
BLEUUID robotHealthCharacteristicUuid("b0b0c7ab-0003-4000-8000-000000000000");
BLECharacteristic *robotHealthCharacteristic;

BLEUUID uartServiceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
BLEService *uartService;

BLE2902 *uartRx2902;
BLEDescriptor *uartRx2901;
BLE2902 *uartTx2902;
BLEDescriptor *uartTx2901;
BLEUUID uartRxCharacteristicUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
BLEUUID uartTxCharacteristicUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
BLECharacteristic *uartRxCharacteristic;
BLECharacteristic *uartTxCharacteristic;

ServerCallbacks *bleServerCallbacks;

CharacteristicCallbacks *bleCharacteristicCallbacks;

BLEServer *getBLEServer() {
    return bleServer;
}

BLEService *getRobotService() {
    return robotService;
}

BLEService *getUARTService() {
    return uartService;
}

BLECharacteristic *getRobotControlCharacteristic() {
    return robotControlCharacteristic;
}

BLECharacteristic *getRobotPositionCharacteristic() {
    return robotPositionCharacteristic;
}

BLECharacteristic *getRobotHealthCharacteristic() {
    return robotHealthCharacteristic;
}

BLECharacteristic *getUARTRxCharacteristic() {
    return uartRxCharacteristic;
}
BLECharacteristic *getUARTTxCharacteristic() {
    return uartTxCharacteristic;
}

void CharacteristicCallbacks::onWrite(BLECharacteristic *bleCharacteristic) {
    if (bleCharacteristic == robotControlCharacteristic) {
        onRobotControl(robotControlCharacteristic->getValue());
    }
}

void bleSetup() {
    bleServerCallbacks = new ServerCallbacks();
    bleCharacteristicCallbacks = new CharacteristicCallbacks();

    BLEDevice::init(BLE_DEVICE_NAME);
    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(bleServerCallbacks);

    batterySetup(bleServer);

    robotService = bleServer->createService(robotServiceUuid);

    robotControl2902 = new BLE2902();
    robotControl2901 = new BLEDescriptor((uint16_t)0x2901);
    robotControl2901->setValue("Robot Control");
    robotControlCharacteristic = robotService->createCharacteristic(robotControlCharacteristicUuid, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    robotControlCharacteristic->setCallbacks(bleCharacteristicCallbacks);
    robotControlCharacteristic->addDescriptor(robotControl2902);
    robotControlCharacteristic->addDescriptor(robotControl2901);

    robotPosition2902 = new BLE2902();
    robotPosition2902->setIndications(true);
    robotPosition2902->setNotifications(true);
    robotPosition2901 = new BLEDescriptor((uint16_t)0x2901);
    robotPosition2901->setValue("Robot Position");
    robotPositionCharacteristic = robotService->createCharacteristic(robotPositionCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    robotPositionCharacteristic->addDescriptor(robotPosition2902);
    robotPositionCharacteristic->addDescriptor(robotPosition2901);

    robotHealth2902 = new BLE2902();
    robotHealth2902->setIndications(true);
    robotHealth2902->setNotifications(true);
    robotHealth2901 = new BLEDescriptor((uint16_t)0x2901);
    robotHealth2901->setValue("Robot Health");
    robotHealthCharacteristic = robotService->createCharacteristic(robotHealthCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    robotHealthCharacteristic->addDescriptor(robotHealth2902);
    robotHealthCharacteristic->addDescriptor(robotHealth2901);

    robotService->start();

    uartService = bleServer->createService(uartServiceUuid);

    uartRx2902 = new BLE2902();
    uartRx2901 = new BLEDescriptor((uint16_t)0x2901);
    uartRx2901->setValue("UART Rx");
    uartRxCharacteristic = uartService->createCharacteristic(uartRxCharacteristicUuid, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
    uartRxCharacteristic->setCallbacks(bleCharacteristicCallbacks);
    uartRxCharacteristic->addDescriptor(uartRx2902);
    uartRxCharacteristic->addDescriptor(uartRx2901);
    uartTx2902 = new BLE2902();
    uartTx2902->setIndications(true);
    uartTx2902->setNotifications(true);
    uartTx2901 = new BLEDescriptor((uint16_t)0x2901);
    uartTx2901->setValue("UART Tx");
    uartTxCharacteristic = uartService->createCharacteristic(uartTxCharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY);
    uartTxCharacteristic->addDescriptor(uartTx2902);
    uartTxCharacteristic->addDescriptor(uartTx2901);

    uartService->start();

    BLEAdvertising *bleAdvertising = BLEDevice::getAdvertising();
    bleAdvertising->addServiceUUID(robotServiceUuid);
    bleAdvertising->addServiceUUID(uartServiceUuid);
    bleAdvertising->start();
}
