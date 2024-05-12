#ifndef LCD_HPP
#define LCD_HPP

#include "SPI.h"

#include "../../components/arduino/libraries/Adafruit_GFX/src/Adafruit_GFX.h"

#include "../../components/arduino/libraries/Adafruit_ILI9341/src/Adafruit_ILI9341.h"

// #define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
// #define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
// #define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
// #define EXAMPLE_PIN_NUM_SCLK           14
// #define EXAMPLE_PIN_NUM_MOSI           13
// #define EXAMPLE_PIN_NUM_LCD_DC         32
// #define EXAMPLE_PIN_NUM_LCD_RST        33
// #define EXAMPLE_PIN_NUM_LCD_CS         5
// #define EXAMPLE_PIN_NUM_BK_LIGHT       26


//
#define TFT_DC 32
#define TFT_CS 15
#define TFT_MOSI 13
#define TFT_CLK 14
#define TFT_RST 33
#define TFT_MISO 12

// hardware SPI
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void init_tft();


#endif // LCD_HPP
