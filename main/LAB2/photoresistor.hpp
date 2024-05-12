#ifndef PHOTO_RESISTOR_HPP
#define PHOTO_RESISTOR_HPP

#include "LAB2/esp_adc.hpp"

#define PIN36   ADC_CHANNEL_0  // GPIO36 connected to the photoresistor

int volt_to_resistance(int voltage);

#endif // PHOTO_RESISTOR_HPP