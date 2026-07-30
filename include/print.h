
#pragma once

#include <Arduino.h>

__attribute__((format(printf, 1, 2))) 
void print(const char* format, ...);
