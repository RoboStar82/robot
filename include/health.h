
#pragma once

#include <BLE2902.h>
#include <BLEService.h>

#include "print.h"

uint8_t *getHealth();

BLECharacteristic *getHealthCharacteristic();

void healthSetup(BLEService *robotService);
