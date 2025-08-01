
#pragma once

#ifndef DEBUG_POSITION
#define DEBUG_POSITION 0
#endif

#ifndef DEBUG_POSITION_TX
#define DEBUG_POSITION_TX 0
#endif

#if ROBOT_HAS_LIDAR

#include <NimBLEDevice.h>

#include "bmx.h"
#include "lidar.h"
#include "print.h"

uint8_t *getPosition();

BLECharacteristic *getPositionCharacteristic();

void positionSetup(BLEService *robotService);

void positionBegin(void *params);

void positionLoop();

#endif
