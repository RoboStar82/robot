
#pragma once

#if ROBOT_HAS_LIDAR

#include <BLE2902.h>
#include <BLEService.h>

#include "bmx.h"
#include "lidar.h"
#include "print.h"

uint8_t *getPosition();

BLECharacteristic *getPositionCharacteristic();

void positionSetup(BLEService *robotService);

void positionBegin(void *params);

void positionLoop();

#endif
