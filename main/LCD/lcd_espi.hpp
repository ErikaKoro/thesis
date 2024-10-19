#ifndef LCD_ESPI_HPP
#define LCD_ESPI_HPP

#include <TFT_eSPI.h>


// Enumeration that contains all possible modes of the DIP SWITCH
enum mode{
    LAB1 = 0,           // 0 0 0 0 on the DIP SWITCH  (0)
    LAB2 = 1,           // 0 0 0 1 on the DIP SWITCH  (1)
    LAB3 = 2,           // 0 0 1 0 on the DIP SWITCH  (2)
    PHOTORES = 3,       // 0 0 1 1 on the DIP SWITCH  (3)
    WAVEFORM = 4,       // 0 1 0 0 on the DIP SWITCH  (4)
    SINE_WAVE = 5,      // 1 0 0 0 on the DIP SWITCH  (8)
    TRIANGLE_WAVE = 6,  // 1 0 0 1 on the DIP SWITCH  (9)
    SAWTOOTH_WAVE = 7,  // 1 0 1 0 on the DIP SWITCH  (10)
    SQUARE_WAVE = 8,    // 1 0 1 1 on the DIP SWITCH  (11)
    NOTHING = 9,        // Nothing state              (12)
    INITIAL = 10        // Initial state              (13)
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
    TFT_eSprite waveform_background = TFT_eSprite(&tft);

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