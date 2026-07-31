
#pragma once

#include <Arduino.h>

#include "print.h"

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
