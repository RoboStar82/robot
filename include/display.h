
#pragma once

#include <Arduino.h>

#include "config.h"

class Display {
   public:
    Display();
    ~Display();

    void begin();

    void clear();

    int length();

    void drawLogo();

    void drawFrameBuffer(uint16_t* buffer, uint16_t width, uint16_t height);

   protected:
};

extern Display display;
