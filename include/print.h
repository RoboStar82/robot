
#pragma once

#include <Arduino.h>

#define print(format, ...) Serial.printf(format, ##__VA_ARGS__)
