
#pragma once

#include <NimBLEDevice.h>

#include "print.h"
#include "version.h"

uint8_t *getHealth();

int getHealthSize();

BLECharacteristic *getHealthCharacteristic();

void healthSetup(BLEService *robotService);
