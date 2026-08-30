
#include <Arduino.h>

#include "print.h"
#include "writer.h"

void print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    writer.vprintf(format, args);
    va_end(args);
}

void print(const uint8_t* buffer, size_t length) {
    writer.write(buffer, length);
}

void vprint(const char* format, va_list args) {
    writer.vprintf(format, args);
}
