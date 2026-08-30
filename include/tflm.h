
#pragma once

#include <Arduino.h>

#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>

#include "config.h"

class TFLM {
   public:
    TFLM();
    virtual ~TFLM() = default;

    void begin();
    void end();

   protected:
};

extern TFLM tflm;

extern void MicroPrintf(const char* format, ...);
extern void VMicroPrintf(const char* format, va_list args);
extern int MicroSnprintf(char* buffer, size_t length, const char* format, ...);
extern int MicroVsnprintf(char* buffer, size_t length, const char* format, va_list args);
