#include "lcd_espi.hpp"
#include <Arduino.h>

/**
 * Create a sprite for each value that has to be updated
 * Configure the display for the lab 1
 */
void setup_display_lab1(sprites *spr_1){
    spr_1->tft.setTextSize(3);

    // how many pixels does the static text occupy
    int sprite_width = spr_1->tft.textWidth("Volt: ");
    spr_1->start_pixel = sprite_width;

    // the remaining pixels are for the value
    // 5 is the first x position of the text
    sprite_width = 320 - (sprite_width + 5);


    // calculate how many pixels the raw value occupies
    // in order to create a sprite with the minimum memory possible
    int raw_width = spr_1->tft.textWidth("9999");

    // create the sprite for the adc raw data
    spr_1->raw.setColorDepth(8);
    // Sprite's dimensions
    spr_1->raw.createSprite(raw_width, 50);
    spr_1->raw.setTextSize(3);

    // create the sprite for the adc voltage
    spr_1->voltage.setColorDepth(8);
    spr_1->voltage.createSprite(sprite_width, 50);
    spr_1->voltage.setTextSize(3);

    spr_1->tft.init();

    // landscape mode
    spr_1->tft.setRotation(1);
    // fill screen with black to erase previous data on LCD
    spr_1->tft.fillScreen(TFT_BLACK);

}


void setup_display_lab2(sprites *spr_2) {

    spr_2->tft.setTextSize(3);
    // how many pixels does the text occupy
    int sprite_width = spr_2->tft.textWidth("MCP Volt: ");
    spr_2->start_pixel = sprite_width;

    // the remaining pixels are for the value
    // 5 is the first x position of the text
    sprite_width = 320 - (sprite_width + 5);

    // calculate how many pixels the raw value occupies
    // in order to create a sprite with the minimum memory possible
    int raw_width = spr_2->tft.textWidth("9999");

    // create the sprite for the adc raw data
    spr_2->raw.setColorDepth(8);
    spr_2->raw.createSprite(raw_width, 50);
    spr_2->raw.setTextSize(3);

    // create the sprite for the adc voltage
    spr_2->voltage.setColorDepth(8);
    spr_2->voltage.createSprite(sprite_width, 50);
    spr_2->voltage.setTextSize(3);

    // create the sprite for the mcp data
    spr_2->mcp.setColorDepth(8);
    spr_2->mcp.createSprite(raw_width, 50);
    spr_2->mcp.setTextSize(3);

    // create the sprite for the mcp voltage
    spr_2->mcp_vol.setColorDepth(8);
    spr_2->mcp_vol.createSprite(sprite_width, 50);
    spr_2->mcp_vol.setTextSize(3);

    spr_2->tft.init();

    // landscape mode
    spr_2->tft.setRotation(1);
    // fill screen with black to erase previous data on LCD
    spr_2->tft.fillScreen(TFT_BLACK);

}


/**
 * Create a static display for the lab 2
 */
void init_lab1(sprites *spr_1){

    // fill screen with black to erase previous data on LCD
    spr_1->tft.fillScreen(TFT_BLACK);
    spr_1->tft.setTextColor(TFT_RED);
    spr_1->tft.setTextSize(3);
    spr_1->tft.drawString("LAB 1", 5, 5);
    spr_1->tft.drawRect(5, 35, 320, 3, TFT_RED);

    spr_1->tft.setTextColor(TFT_GREEN);
    spr_1->tft.drawString("Raw: ", 5, 60);
    spr_1->tft.drawString("Volt: ", 5, 110);   

}


/**
 * Create a static display for the lab 2
 */
void init_lab2(sprites *spr_2){

    // fill screen with black to erase previous data on LCD
    spr_2->tft.fillScreen(TFT_BLACK);
    spr_2->tft.setTextColor(TFT_RED);
    spr_2->tft.setTextSize(3);
    spr_2->tft.drawString("LAB 2", 5, 5);
    spr_2->tft.drawRect(5, 35, 320, 3, TFT_RED);

    spr_2->tft.setTextColor(TFT_GREEN);
    spr_2->tft.drawString("Raw: ", 5, 60);
    spr_2->tft.drawString("Volt: ", 5, 110);
    spr_2->tft.drawString("MCP Raw: ", 5, 160);
    spr_2->tft.drawString("MCP Volt: ", 5, 210);    

}

