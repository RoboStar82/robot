
#pragma once

#include <BLE2902.h>
#include <BLEService.h>

#include "print.h"

uint8_t *getRobotHealth();

BLECharacteristic *getRobotHealthCharacteristic();

void robotHealthSetup(BLEService *robotService);
