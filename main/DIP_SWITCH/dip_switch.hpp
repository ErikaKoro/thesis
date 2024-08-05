#ifndef DIP_SWITCH_HPP
#define DIP_SWITCH_HPP

extern "C" {
    #include "hal/gpio_types.h"
}

#define DIP_SWITCH_PIN_1  GPIO_NUM_4  // GPIO 2 controls the most significant bit of the DIP SWITCH
#define DIP_SWITCH_PIN_2  GPIO_NUM_9 
#define DIP_SWITCH_PIN_3  GPIO_NUM_22
#define DIP_SWITCH_PIN_4  GPIO_NUM_21 // GPIO 21 controls the least significant bit of the DIP SWITCH

#define GPIO_INPUT_PIN_SEL ((1ULL<<DIP_SWITCH_PIN_1) | (1ULL<<DIP_SWITCH_PIN_2) | (1ULL<<DIP_SWITCH_PIN_3) | (1ULL<<DIP_SWITCH_PIN_4))

void init_dip_switch();

#endif // DIP_SWITCH_HPP