
#pragma once

#include <Arduino.h>
#include <freertos/task.h>

#include <iarduino_Position_BMX055.h>

iarduino_Position_BMX055 *getBMX();

void bmxSetup();

void bmxBegin(void *params);

void bmxCalibrate();

void bmxLoop();
