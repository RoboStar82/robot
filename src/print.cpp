
#include "print.h"

void print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    Serial.printf(format, args);
    va_end(args);
}
