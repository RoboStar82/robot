
#pragma once

#include <Arduino.h>
#include <freertos/task.h>
#include <iarduino_Position_BMX055.h>

#include "print.h"

iarduino_Position_BMX055 *getBMX();

void bmxSetup();

void bmxBegin(void *params);

void bmxCalibrate(int time = 30000);

void bmxLoop();
