#include "lcd_espi.hpp"
#include <Arduino.h>

/*
 * Calculates the sprites' dimensions and makes the sprites' configurations
*/
void setup_display(sprites *spr_2, String header, mode current_mode) {

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

    spr_2->tft.init();

    // landscape mode
    spr_2->tft.setRotation(1);
    // fill screen with black to erase previous data on LCD
    spr_2->tft.fillScreen(TFT_BLACK);

}


void setup_display_lab3(sprites *spr_3){
    
        spr_3->tft.setTextSize(3);
    
        // how many pixels does the text occupy
        int sprite_width = spr_3->tft.textWidth("VRMS,CRMS: ");
        spr_3->start_pixel = sprite_width;
    
        // the remaining pixels are for the value
        // 5 is the first x position of the text
        sprite_width = 320 - (sprite_width + 5);
    
        // create the sprite for voltage and current rms
        spr_3->rms.setColorDepth(8);
        spr_3->rms.createSprite(sprite_width, 50);
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
    
        spr_3->tft.init();
    
        // landscape mode
        spr_3->tft.setRotation(1);
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
    spr_2->tft.drawRect(5, 35, 320, 3, TFT_RED);
    spr_2->tft.setTextColor(TFT_GREEN);


    if(current_mode == LAB2 || current_mode == LAB1){
        spr_2->tft.drawString("Raw: ", 5, 60);
        spr_2->tft.drawString("Volt: ", 5, 110);
    }
    
    if(current_mode == LAB2){
        spr_2->tft.drawString("MCP Raw: ", 5, 160);
        spr_2->tft.drawString("MCP Volt: ", 5, 210);
    }

    if(current_mode == LAB3){
        spr_2->tft.drawString("VRMS,CRMS: ", 5, 60);
        spr_2->tft.drawString("Active: ", 5, 110);
        spr_2->tft.drawString("Apparent: ", 5, 160);
        spr_2->tft.drawString("P.factor: ", 5, 210);
    }
    

        

}

