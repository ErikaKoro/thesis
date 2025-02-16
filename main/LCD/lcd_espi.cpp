#include "lcd_espi.hpp"
#include <Arduino.h>

/*
 * Calculates the sprites' dimensions and makes the sprites' configurations
*/
void setup_display(sprites *spr_2, String header, mode current_mode){

    // fill screen with black to erase previous data on LCD
    spr_2->tft.fillScreen(TFT_BLACK);

    spr_2->tft.setTextSize(3);

    // how many pixels does the text occupy
    int sprite_width = spr_2->tft.textWidth(header);  // "MCP Volt: " , "Volt: " 
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

    if(current_mode == LAB2){

        // create the sprite for the mcp data
        spr_2->mcp.setColorDepth(8);
        spr_2->mcp.createSprite(raw_width, 50);
        spr_2->mcp.setTextSize(3);
        
        // create the sprite for the mcp voltage
        spr_2->mcp_vol.setColorDepth(8);
        spr_2->mcp_vol.createSprite(sprite_width, 50);
        spr_2->mcp_vol.setTextSize(3);
    }

}


void setup_display_lab3(sprites *spr_3){
    
        spr_3->tft.setTextSize(3);
    
        // how many pixels does the text occupy
        int sprite_width = spr_3->tft.textWidth("Apparent: ");
        spr_3->start_pixel = sprite_width;
    
        // the remaining pixels are for the value
        // 5 is the first x position of the text
        sprite_width = 320 - (sprite_width + 5);
    
        // create the sprite for voltage and current rms
        spr_3->rms.setColorDepth(8);
        spr_3->rms.createSprite(sprite_width + 80, 50);
        spr_3->rms.setTextSize(3);
    
        // create the sprite for the active power
        spr_3->active.setColorDepth(8);
        spr_3->active.createSprite(sprite_width, 50);
        spr_3->active.setTextSize(3);
    
        // create the sprite for the apparent power
        spr_3->apparent.setColorDepth(8);
        spr_3->apparent.createSprite(sprite_width, 50);
        spr_3->apparent.setTextSize(3);
        
        // create the sprite for the power factor
        spr_3->factor.setColorDepth(8);
        spr_3->factor.createSprite(sprite_width, 50);
        spr_3->factor.setTextSize(3);
    
        // fill screen with black to erase previous data on LCD
        spr_3->tft.fillScreen(TFT_BLACK);
    
    
}


/**
 * Create a static display for the lab 2
 */
void init_lab(sprites *spr_2, mode current_mode, String header){

    // fill screen with black to erase previous data on LCD
    spr_2->tft.fillScreen(TFT_BLACK);
    spr_2->tft.setTextColor(TFT_RED);
    spr_2->tft.setTextSize(3);
    spr_2->tft.drawString(header, 5, 5);  // LAB 1, LAB 2

    if(current_mode != NOTHING){
        spr_2->tft.drawRect(5, 35, 320, 3, TFT_RED);
    }
    spr_2->tft.setTextColor(TFT_GREEN);

    if(current_mode == LAB1){
        spr_2->tft.drawString("Raw: ", 5, 60);
        spr_2->tft.drawString("Volt: ", 5, 110);

    } else if(current_mode == LAB2){
        spr_2->tft.drawString("Raw: ", 5, 60);
        spr_2->tft.drawString("Volt: ", 5, 110);
        spr_2->tft.drawString("MCP Raw: ", 5, 160);
        spr_2->tft.drawString("MCP Volt: ", 5, 210);

    } else if(current_mode == LAB3){
        spr_2->tft.drawString("V,I: ", 5, 60);
        spr_2->tft.drawString("Active: ", 5, 110);
        spr_2->tft.drawString("Apparent: ", 5, 160);
        spr_2->tft.drawString("P.factor: ", 5, 210);

    } else if(current_mode == PHOTORES){
        spr_2->tft.drawString("raw: ", 5, 60);
        spr_2->tft.drawString("volt: ", 5, 110);

    } else if(current_mode == SINE_WAVE){
        int text_width = spr_2->tft.textWidth("Sine Wave");
        int center_pixel = (320 - text_width)/2;
        spr_2->tft.drawString("Sine Wave", center_pixel, 120);

    } else if(current_mode == TRIANGLE_WAVE){
        int text_width = spr_2->tft.textWidth("Triangle Wave");
        int center_pixel = (320 - text_width)/2;
        spr_2->tft.drawString("Triangle Wave", center_pixel, 120);

    } else if(current_mode == SAWTOOTH_WAVE){
        int text_width = spr_2->tft.textWidth("Sawtooth Wave");
        int center_pixel = (320 - text_width)/2;
        spr_2->tft.drawString("Sawtooth Wave", center_pixel, 120);

    } else if(current_mode == SQUARE_WAVE){
        int text_width = spr_2->tft.textWidth("Square Wave");
        int center_pixel = (320 - text_width)/2;
        spr_2->tft.drawString("Square Wave", center_pixel, 120);

    } else if(current_mode == NOTHING){
        int text_width = spr_2->tft.textWidth("Error 404");
        int center_pixel = (320 - text_width)/2;
        spr_2->tft.drawString("Error 404", center_pixel, 80);

        text_width = spr_2->tft.textWidth("Mode Not Found :(");
        center_pixel = (320 - text_width)/2;
        spr_2->tft.drawString("Mode Not Found :(", center_pixel, 120);

    }

}

