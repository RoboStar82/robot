
#include "print.h"

size_t debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    size_t r = Serial.vprintf(format, args);
    va_end(args);
    return r;
}

size_t print(String value) {
    return Serial.print(value);
}

size_t print(const char *value) {
    return Serial.print(value);
}

size_t print(int value) {
    return Serial.print(value);
}

size_t print(float value) {
    return Serial.print(value);
}

size_t println(String value) {
    return Serial.println(value);
}

size_t println(const char *value) {
    return Serial.println(value);
}

size_t println(int value) {
    return Serial.println(value);
}

size_t println(float value) {
    return Serial.println(value);
}

size_t println() {
    return Serial.println();
}
