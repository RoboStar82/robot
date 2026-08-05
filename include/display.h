
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

   protected:
};

extern Display display;
