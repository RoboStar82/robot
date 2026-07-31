
#include "config.h"

#ifdef ROBOT_HAS_DISPLAY

#include "display.h"

Display display;

#ifdef ROBOT_HAS_DISPLAY_ST7735
#include <Adafruit_ST7735.h>
Adafruit_ST7735 displayST7735 = Adafruit_ST7735(
    DISPLAY_ST7735_CS_PIN,
    DISPLAY_ST7735_DC_PIN,
    DISPLAY_ST7735_MOSI_PIN,
    DISPLAY_ST7735_SCK_PIN,
    DISPLAY_ST7735_RST_PIN);
#endif

#ifdef ROBOT_HAS_DISPLAY_GDEY0213Z98
#include <GxEPD2_3C.h>
SPISettings spiSettingsGDEY0213Z98;
SPIClass spiGDEY0213Z98(DISPLAY_GDEY0213Z98_MOSI_PIN, DISPLAY_GDEY0213Z98_MISO_PIN, DISPLAY_GDEY0213Z98_SCK_PIN, DISPLAY_GDEY0213Z98_CS_PIN);
GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> displayGDEY0213Z98(GxEPD2_213_Z98c(
    DISPLAY_GDEY0213Z98_CS_PIN,
    DISPLAY_GDEY0213Z98_DC_PIN,
    DISPLAY_GDEY0213Z98_RST_PIN,
    DISPLAY_GDEY0213Z98_BUSY_PIN));
#endif

#ifdef ROBOT_HAS_DISPLAY_E0213A367
#define DISABLE_SDCARD
#include <heltec-eink-modules.h>
#undef DISABLE_SDCARD
E0213A367 displayE0213A367;
#endif

Display::Display() {}

Display::~Display() {}

void Display::begin() {
    bool logo = false;
#ifdef ROBOT_HAS_DISPLAY_ST7735
    displayST7735.initR(INITR_MINI160x80_PLUGIN);
    displayST7735.setRotation(1);
    displayST7735.fillScreen(ST77XX_BLACK);
    drawLogo();
    delay(100);
    pinMode(DISPLAY_ST7735_LED_PIN, OUTPUT);
    digitalWrite(DISPLAY_ST7735_LED_PIN, LOW);
#endif
#ifdef ROBOT_HAS_DISPLAY_GDEY0213Z98
    spiGDEY0213Z98.begin();
    displayGDEY0213Z98.epd2.selectSPI(spiGDEY0213Z98, spiSettingsGDEY0213Z98);
    displayGDEY0213Z98.init();
    displayGDEY0213Z98.clearScreen();
    delay(1000);
    displayGDEY0213Z98.hibernate();
#endif
#ifdef ROBOT_HAS_DISPLAY_E0213A367
    pinMode(PIN_DISPLAY_RST, OUTPUT);
    displayE0213A367.begin();
    displayE0213A367.landscape();
    displayE0213A367.fullscreen();
    drawLogo();
#endif
}

void Display::clear() {
#ifdef ROBOT_HAS_DISPLAY_ST7735
    displayST7735.fillScreen(ST77XX_BLACK);
#endif
#ifdef ROBOT_HAS_DISPLAY_GDEY0213Z98
    displayGDEY0213Z98.clearScreen();
#endif
#ifdef ROBOT_HAS_DISPLAY_E0213A367
    displayE0213A367.clear();
#endif
}

int Display::length() {
    return 0
#ifdef ROBOT_HAS_DISPLAY_ST7735
           + 1
#endif
#ifdef ROBOT_HAS_DISPLAY_GDEY0213Z98
           + 1
#endif
#ifdef ROBOT_HAS_DISPLAY_E0213A367
           + 1
#endif
        ;
}

void Display::drawLogo() {
#ifdef ROBOT_HAS_DISPLAY_ST7735
#include "../images/r_82_fb_102x36.pgm.h"
    {
        int x0 = 30;
        int y0 = 25;
        int x1 = x0 + r_82_fb_102x36_width;
        int y1 = y0 + r_82_fb_102x36_height;
        displayST7735.startWrite();
        int offsetByte = 0;
        uint8_t previousByte = 0;
        uint16_t previousColor = displayST7735.color565(previousByte, previousByte, previousByte);
        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                uint8_t currentByte = r_82_fb_102x36_bitmap[offsetByte];
                uint16_t currentColor;
                if (currentByte == previousByte) {
                    currentColor = previousColor;
                } else {
                    currentColor = displayST7735.color565(currentByte, currentByte, currentByte);
                    previousColor = currentColor;
                    previousByte = currentByte;
                }
                displayST7735.drawPixel(x, y, currentColor);
                offsetByte++;
            }
        }
        displayST7735.endWrite();
    }
#endif
#ifdef ROBOT_HAS_DISPLAY_E0213A367
#include "../images/r_82_f_204x72.xbm"
    {
        int x0 = 30;  // 22 // displayE0213A367.centerX() - r_82_f_204x72_width / 2;
        int x1 = x0 + r_82_f_204x72_width;
        int y0 = displayE0213A367.centerY() - r_82_f_204x72_height / 2;
        int y1 = y0 + r_82_f_204x72_height;
        int offsetByte = 0, offsetBits = 0, currentByte = 0;
        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                if (!offsetBits) {
                    currentByte = r_82_f_204x72_bits[offsetByte];
                }
                int color = (currentByte >> offsetBits) & 1;
                offsetBits++;
                if (offsetBits > 7) {
                    offsetBits = 0;
                    offsetByte++;
                }
                if (color) {
                    displayE0213A367.drawPixel(x, y, BLACK);
                }
            }
            if (offsetBits) {
                offsetBits = 0;
                offsetByte++;
            }
        }
        displayE0213A367.update();
    }
#endif
}

#endif
