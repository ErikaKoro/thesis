#ifndef LCD_ESPI_HPP
#define LCD_ESPI_HPP

#include <TFT_eSPI.h>


/*
* Create a sprite for each value that has to be updated
*/
struct sprites{

    TFT_eSPI tft = TFT_eSPI();


    // Sprites for lab 2
    // raw adc data
    TFT_eSprite raw = TFT_eSprite(&tft);
    // adc voltage
    TFT_eSprite voltage = TFT_eSprite(&tft);
    // mcp data
    TFT_eSprite mcp = TFT_eSprite(&tft);
    // mcp voltage
    TFT_eSprite mcp_vol= TFT_eSprite(&tft);
    // the starting pixel is the end of the written text
    int start_pixel;
};

void setup_display_lab1(sprites *spr_1);
void setup_display_lab2(sprites *spr_2);

void init_lab1(sprites *spr_1);
void init_lab2(sprites *spr_2);


// MCP
// void update_external_adc(int data, int voltage_value);

#endif