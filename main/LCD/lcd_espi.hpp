#ifndef LCD_ESPI_HPP
#define LCD_ESPI_HPP

#include <TFT_eSPI.h>


// Enumeration that contains all possible modes of the DIP SWITCH
enum mode{
    LAB1 = 0,   // 0 0 0 0 on the DIP SWITCH
    LAB2 = 1,   // 0 1 0 0 on the DIP SWITCH
    LAB3 = 2,   // 1 0 0 0 on the DIP SWITCH
    PHOTORES = 3,  // 1 1 0 0 on the DIP SWITCH
    BUZZER = 4, // 1 1 1 1 on the DIP SWITCH
    WAVEFORM = 5 // 0 1 1 0 on the DIP SWITCH
};


/*
* Create a sprite for each value that has to be updated
*/
struct sprites{

    TFT_eSPI tft = TFT_eSPI();

    // raw adc data
    TFT_eSprite raw = TFT_eSprite(&tft);
    // adc voltage
    TFT_eSprite voltage = TFT_eSprite(&tft);
    // mcp data
    TFT_eSprite mcp = TFT_eSprite(&tft);
    // mcp voltage
    TFT_eSprite mcp_vol = TFT_eSprite(&tft);

    // VRMS and CRMS
    TFT_eSprite rms = TFT_eSprite(&tft);
    TFT_eSprite active = TFT_eSprite(&tft);
    TFT_eSprite apparent = TFT_eSprite(&tft);
    TFT_eSprite factor = TFT_eSprite(&tft);

    // waveform sprite
    TFT_eSprite waveform = TFT_eSprite(&tft);

    // the starting pixel is the end of the written text
    int start_pixel;
};

void setup_display_lab3(sprites *spr_3);

// void setup_display_lab1(sprites *spr_1);
// void setup_display_lab2(sprites *spr_2);
void setup_display(sprites *spr_2, String header, mode current_mode);
// void init_lab1(sprites *spr_1);
// void init_lab(sprites *spr_2);
void init_lab(sprites *spr_2, mode current_mode, String header);


// MCP
// void update_external_adc(int data, int voltage_value);

#endif