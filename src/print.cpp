
#include "print.h"

size_t debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    va_list copy;
    va_copy(copy, args);
    char buffer[64];
    char *buffer2 = NULL;
    int r = vsnprintf(buffer, sizeof(buffer), format, copy);
    va_end(copy);
    if (r < 0) {
        va_end(args);
        return 0;
    }
    if (r >= (int)sizeof(buffer)) {
        buffer2 = (char *)malloc(r + 1);
        if (buffer2 == NULL) {
            va_end(args);
            return 0;
        }
        r = vsnprintf(buffer2, r + 1, format, args);
    } else {
        buffer2 = buffer;
    }
    va_end(args);
    r = Serial.write((uint8_t *)buffer2, r);
    if (buffer2 != buffer) {
        free(buffer2);
    }
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
