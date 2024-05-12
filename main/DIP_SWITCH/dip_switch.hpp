#ifndef DIP_SWITCH_HPP
#define DIP_SWITCH_HPP

extern "C" {
    #include "hal/gpio_types.h"
}

#define DIP_SWITCH_PIN_1  GPIO_NUM_2  // GPIO 2 controls the most significant bit of the DIP SWITCH
#define DIP_SWITCH_PIN_2  GPIO_NUM_0  // GPIO 0 controls the least significant bit of the DIP SWITCH

#define GPIO_INPUT_PIN_SEL ((1ULL<<DIP_SWITCH_PIN_1) | (1ULL<<DIP_SWITCH_PIN_2))

void init_dip_switch();

#endif // DIP_SWITCH_HPP