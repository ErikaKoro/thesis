#ifndef POWER_METER_HPP
#define POWER_METER_HPP

#include "../LAB2/esp_adc.hpp"

#include "Arduino.h"


#define vPin ADC_CHANNEL_1  // 37
#define iPin ADC_CHANNEL_2  // 38

#define nSamples 400
#define vOffset 1885
#define vStep 0.012
#define iOffset 1137
#define iStep 0.000659

typedef struct{
    double activePower;
    double apparentPower;
    double powerFactor;
}powerMes;

typedef struct{
    
    double vRms;
    double aRms;
    
    double *vCalibrated;
    double *cCalibrated;    
}powerArrays;


powerMes power_meter(powerArrays *powerArrays, adc_oneshot_unit_handle_t adc1_handle);

#endif