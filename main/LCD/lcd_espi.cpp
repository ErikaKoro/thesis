#include <TFT_eSPI.h>
#include "lcd_espi.hpp"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);

void setup_display(void) {
  tft.init();
  tft.setRotation(1);
  img.setColorDepth(8);
  img.createSprite(320, 240);
}

int x = 0;

void test(){
img.fillCircle(x, 36, 30, TFT_BLACK);
    x = x + 1;
    
    if(x > 320) x = 0;

    img.fillCircle(x, 36, 30, TFT_RED);
    img.fillRect(18, 70, 100, 100, TFT_BLACK);
    img.drawString(String(x), 20, 74, 7);

    img.pushSprite(0,0);
}