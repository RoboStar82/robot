
#include "display.h"

Display display;

#if ROBOT_HAS_DISPLAY_GDEY0213Z98
#include <GxEPD2_3C.h>
GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> displayGxEPD2(GxEPD2_213_Z98c(
    DISPLAY_GDEY0213Z98_CS_PIN,
    DISPLAY_GDEY0213Z98_DC_PIN,
    DISPLAY_GDEY0213Z98_RES_PIN,
    DISPLAY_GDEY0213Z98_BUSY_PIN));
#endif

Display::Display() {}

Display::~Display() {}

void Display::begin() {}
