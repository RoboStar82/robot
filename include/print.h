
#pragma once

#include <Arduino.h>

size_t debug(const char *format, ...);

size_t print(String value);

size_t print(const char *value);

size_t print(int value);

size_t print(float value);

size_t println(String value);

size_t println(const char *value);

size_t println(int value);

size_t println(float value);

size_t println();
