
#include <Arduino.h>

#if ROBOT_HAS_DISPLAY_GDEY0213Z98
#ifndef DISPLAY_GDEY0213Z98_CS_PIN
#define DISPLAY_GDEY0213Z98_CS_PIN -1
#endif
#ifndef DISPLAY_GDEY0213Z98_DC_PIN
#define DISPLAY_GDEY0213Z98_DC_PIN -1
#endif
#ifndef DISPLAY_GDEY0213Z98_RES_PIN
#define DISPLAY_GDEY0213Z98_RES_PIN -1
#endif
#ifndef DISPLAY_GDEY0213Z98_BUSY_PIN
#define DISPLAY_GDEY0213Z98_BUSY_PIN -1
#endif
#endif

class Display {
   public:
    Display();
    ~Display();

    void begin();

   protected:
};

extern Display display;
