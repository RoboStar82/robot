
#pragma once

#include <Arduino.h>

#ifdef ROBOT_HAS_DISPLAY_ST7735
#include <Adafruit_ST7735.h>
extern Adafruit_ST7735 displayST7735;
#endif

#ifdef ROBOT_HAS_DISPLAY_GDEY0213Z98
#include <GxEPD2_3C.h>
extern SPIClass spiGDEY0213Z98;
extern SPISettings spiSettingsGDEY0213Z98;
extern GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> displayGDEY0213Z98;
#endif

#ifdef ROBOT_HAS_DISPLAY_E0213A367
#define DISABLE_SDCARD
#include <heltec-eink-modules.h>
#undef DISABLE_SDCARD
extern E0213A367 displayE0213A367;
#endif

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
