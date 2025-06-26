
#pragma once

#include <BLE2902.h>
#include <BLEService.h>

#include "bmx.h"
#include "lidar.h"
#include "print.h"

uint8_t *getRobotPosition();

BLECharacteristic *getRobotPositionCharacteristic();

void robotPositionSetup(BLEService *robotService);

void robotPositionBegin(void *params);

void robotPositionLoop();
