
#include <Arduino.h>

#include "print.h"
#ifdef ROBOT_HAS_WRITER
#include "writer.h"
#endif

void print(const char* format, ...) {
    va_list args;
    va_start(args, format);
#ifdef ROBOT_HAS_WRITER
    writer.vprintf(format, args);
#else
    Serial.vprintf(format, args);
#endif
    va_end(args);
}

void print(const uint8_t* buffer, size_t length) {
#ifdef ROBOT_HAS_WRITER
    writer.write(buffer, length);
#else
    Serial.write(buffer, length);
#endif
}

void vprint(const char* format, va_list args) {
#ifdef ROBOT_HAS_WRITER
    writer.vprintf(format, args);
#else
    Serial.vprintf(format, args);
#endif
}
