
#pragma once

#include <NimBLEDevice.h>

#include "print.h"

uint8_t *getHealth();

BLECharacteristic *getHealthCharacteristic();

void healthSetup(BLEService *robotService);
