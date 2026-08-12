
#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "config.h"

#ifdef ROBOT_HAS_BLE

#include "ble.h"

#ifdef ROBOT_HAS_CONTROLLER
#include "controller.h"
#endif

#ifdef ROBOT_HAS_SETTINGS
#include "settings.h"
#endif

#include "delay.h"
#include "print.h"

BLE ble;

BLE::BLE() {}

BLE::~BLE() {}

void BLE::begin() {
    if (!started) {
        started = true;
        BLEDevice::init("");
#ifdef BLE_SECURITY_PASSKEY
        BLEDevice::setSecurityAuth(true, true, true);
        BLEDevice::setSecurityPasskey(BLE_SECURITY_PASSKEY);
        BLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
#endif
        server = BLEDevice::createServer();
        server->setCallbacks(this);
        advertising = BLEDevice::getAdvertising();
        advertising->setName(BLE_DEVICE_NAME);
        advertising->setAdvertisingCompleteCallback(onAdvertisingComplete);
        battery.begin();
        robot.begin();
        uart.begin();
    }
    if (!hasClient) {
        advertisingStart();
    }
#ifdef ROBOT_HAS_BLE_CONTROLLER
    controllerStart();
#endif
}

void BLE::advertisingStart() {
    if (!advertising->isAdvertising()) {
        print("[BLE] advertising start\n");
        advertising->start(9999);
    }
}

void BLE::advertisingStop() {
    print("[BLE] advertising stop\n");
    advertising->stop();
}

void BLE::controllerStart() {
    if (!controllerStarted) {
        xTaskCreate(taskController, "ble_controller_task", 4096, NULL, 1, &controllerStarted);
    }
}

void BLE::controllerStop() {
    if (controllerStarted) {
        vTaskDelete(controllerStarted);
        controllerStarted = nullptr;
    }
}

void BLE::scanStart() {
    if (!scan) {
        scan = BLEDevice::getScan();
        scan->setScanCallbacks(this);
        scan->setActiveScan(false);
        scan->setInterval(100);
        scan->setWindow(100);
    }
    if (!scan->isScanning()) {
        print("[BLE] scan start\n");
        scan->start(0, false, true);
    }
}

void BLE::scanPause() {
    if (scan && scan->isScanning()) {
        print("[BLE] scan pause\n");
        scan->stop();
    }
}

void BLE::scanResume() {
    if (scan && !scan->isScanning()) {
        print("[BLE] scan resume\n");
        scan->start(0, true, false);
    }
}

void BLE::scanStop() {
    if (scan && scan->isScanning()) {
        print("[BLE] scan stop\n");
        scan->stop();
    }
}

void BLE::end() {
#ifdef ROBOT_HAS_BLE_CONTROLLER
    controllerStop();
#endif
    print("[BLE] end\n");
    battery.end();
    robot.end();
    uart.end();
    BLEDevice::deinit();
    advertising = nullptr;
    server = nullptr;
    client = nullptr;
    scan = nullptr;
    started = false;
}

void BLE::onConnect(BLEServer* server, BLEConnInfo& info) {
    print("[BLE] connected\n");
    advertisingStop();
    hasClient = true;
}

void BLE::onDisconnect(BLEServer* server, BLEConnInfo& info, int reason) {
    print("[BLE] disconnected\n");
    hasClient = false;
}

void BLE::onConnect(BLEClient* client) {
    print("[BLE] client connected\n");
}

void BLE::onConnectFail(BLEClient* client, int reason) {
    print("[BLE] client connect fail\n");
    scanResume();
}

void BLE::onDisconnect(BLEClient* client, int reason) {
    print("[BLE] client disconnected\n");
    hasController = false;
}

void BLE::onDiscovered(const BLEAdvertisedDevice* advertised) {
    std::string name = advertised->getName();
    if (name.length() > 0) {
        print("[BLE] discovered %s\n", advertised->getName().c_str());
        if (advertised->isConnectable() && advertised->isAdvertisingService(hidServiceUuid)) {
            xQueueSendMS(advertisedQueue, &advertised, 0);
            scanPause();
        }
    }
}

void BLE::onScanEnd(const BLEScanResults& results, int reason) {
    print("[BLE] scan end\n");
}

void BLE::onAdvertisingComplete(BLEAdvertising* advertising) {
    print("[BLE] advertising end\n");
}

void BLE::onInputNotify(uint8_t* data, size_t length) {
#ifdef ROBOT_HAS_CONTROLLER
    controller.onInput(data, length, controllerVendorId, controllerProductId);
#endif
}

void BLE::onReportNotify(BLERemoteCharacteristic* characteristic, uint8_t* data, size_t length, bool notify) {
    ble.onInputNotify(data, length);
}

bool BLE::onConnParamsUpdateRequest(BLEClient* client, const ble_gap_upd_params* params) {
    return true;
}

bool BLE::controllerConnect(const BLEAdvertisedDevice* advertised) {
    bool r = controllerConnect(advertised->getAddress());
#ifdef ROBOT_HAS_SETTINGS
    if (r) {
        settings.setControllerAddress(String(advertised->getAddress().toString().c_str()));
    }
#endif
    return r;
}

bool BLE::controllerConnect(const BLEAddress address) {
    print("[BLE] connect %s\n", address.toString().c_str());
    if (!client) {
        client = BLEDevice::getDisconnectedClient();
        if (!client) {
            client = BLEDevice::createClient();
        }
        client->setClientCallbacks(this, false);
        client->setConnectTimeout(9999);
    }
    client->setConnectionParams(16, 32, 0, 100);
    if (client->connect(address, false)) {
        controllerVendorId = 0x00;
        controllerProductId = 0x00;
        if (BLERemoteService* service = client->getService(infoServiceUuid)) {
            print("[BLE] info service %s\n", service->getUUID().toString().c_str());
            if (BLERemoteCharacteristic* characteristic = service->getCharacteristic(infoPnPIdUuid)) {
                print("[BLE] PhP ID characteristic %s\n", characteristic->getUUID().toString().c_str());
                if (characteristic->canRead()) {
                    if (NimBLEAttValue value = characteristic->readValue()) {
                        if (value.length() > 4) {
                            controllerVendorId = value[1] | (value[2] << 8);
                            controllerProductId = value[3] | (value[4] << 8);
                            print("[BLE] vendor:product=%04x:%04x\n", controllerVendorId, controllerProductId);
                        }
                    }
                }
            }
        }
        if (BLERemoteService* service = client->getService(hidServiceUuid)) {
            print("[BLE] HID service %s\n", service->getUUID().toString().c_str());
            if (service->getCharacteristic(hidReportDataUuid)) {
                for (BLERemoteCharacteristic* characteristic : service->getCharacteristics()) {
                    if (characteristic->getUUID() == hidReportDataUuid && characteristic->canNotify()) {
                        print("[BLE] HID characteristic %s\n", characteristic->getUUID().toString().c_str());
                        if (characteristic->subscribe(true, onReportNotify)) {
                            hasController = true;
                        }
                    }
                }
                if (hasController) {
                    client->updateConnParams(16, 32, 1, 100);
                    return true;
                }
            }
        }
        print("[BLE] disconnect %s\n", address.toString().c_str());
        client->disconnect();
    }
    scanResume();
    return false;
}

void BLE::setControllerAddress(String address) {
    controllerAddress = address;
}

void BLE::taskController() {
    controllerAddress = settings.getControllerAddress();
    const BLEAdvertisedDevice* advertised = nullptr;
    while (true) {
        if (!hasController) {
            if (controllerAddress.length()) {
                controllerConnect(BLEAddress(std::string(controllerAddress.c_str()), BLE_ADDR_PUBLIC));
            } else if (xQueueReceiveMS(advertisedQueue, &advertised, 1000)) {
                controllerConnect(advertised);
            } else {
                scanStart();
            }
        } else {
            vTaskDelayMS(1000);
        }
    }
}

void BLE::taskController(void* arg) {
    ble.taskController();
}

#endif
