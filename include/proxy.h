
#pragma once

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

class Proxy : Stream {
   public:
    Proxy();
    ~Proxy();

    void begin();
    void end();

    inline int available() override;
    inline int peek() override;
    inline int read() override;
    inline size_t write(uint8_t c) override;
    inline size_t write(const uint8_t* buffer, size_t length) override;
    inline void flush() override;

    void task();

    using Print::print;
    using Print::printf;
    using Print::println;
    using Print::vprintf;
    using Print::write;

   protected:
    TaskHandle_t taskStarted = nullptr;

    static inline void task(void* arg);
};

extern Proxy proxy;
