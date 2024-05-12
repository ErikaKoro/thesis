#include "dip_switch.hpp"

extern "C"{
    #include "driver/gpio.h"
}


void init_dip_switch(){
    
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;

    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;

    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    //configure GPIO with the given settings
    gpio_config(&io_conf);
}
