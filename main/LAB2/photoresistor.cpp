#include "photoresistor.hpp"
#include "Arduino.h"

int volt_to_resistance(int voltage){
    float voltage_float = (float)voltage;
    return 10000 * ((3291.0 / voltage) - 1);
}

// int resistance_to_lux(int resistance){
    
// }
