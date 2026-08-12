
#include <Arduino.h>

#if true
#include "print.h"
#endif

void print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    RobotSerial.vprintf(format, args);
    va_end(args);
}
