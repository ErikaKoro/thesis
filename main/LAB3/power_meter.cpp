#include "power_meter.hpp"


extern "C"{
    #include <inttypes.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <math.h>

}



/**
 * 
 * @brief Function that converts the raw data from the ADC to voltage
 * @param Arr: Array with the raw data from the ADC
 * @param step: Step of the conversion. It is calculated by the formula: Vpp / (Max_decimal_value - Min_decimal_value). Vpp is the peak-to-peak voltage of the voltage signal = 33.94V and the range of the read
 * values is 3150 - 620 = 2530. For the current signal, Vpp = 0.39V, the read values are 36 and the step is 0.000659.
 * @param offset: Offset of the conversion. It is calculated by the formula: (Max_decimal_value + Min_decimal_value) / 2. For the voltage signal, the offset is 1885 and for the current signal,
 * the offset is 1137.
 * @return double*: Array with the converted values to voltage
 *  
*/

powerMes power_meter(powerArrays *powerArrays, adc_oneshot_unit_handle_t adc1_handle, int time_delay){

    uint64_t start, stop;

    powerMes powerMes;

    int nSamples = 120000 / time_delay;
    int vOffset;
    int iOffset;
    double vStep;
    double iStep;

    int measure;
    int ret = adc_oneshot_read(adc1_handle, vPin, &measure);
    // uint16_t measure = analogRead(vPin);

    if(ret != ESP_OK){
        Serial.println("Error reading raw value");
        return powerMes;
    }

    double vRmsSum = 0;
    double aRmsSum = 0;
    double powerSum = 0;

    int max_v = 0;
    int min_v = 4096;
    int max_i = 0;
    int min_i = 4096;

    int vltg;
    int crnt;

    // measure the voltage for 20ms to calibrate the voltage signal
    for (int i = 0; i < 200; i++){
        start = micros();
        
        adc_oneshot_read(adc1_handle, vPin, &vltg);
        adc_oneshot_read(adc1_handle, iPin, &crnt);

        if (vltg > max_v)
            max_v = vltg;
        if (vltg < min_v)
            min_v = vltg;

        if (crnt > max_i)
            max_i = crnt;
        if (crnt < min_i)
            min_i = crnt;

        delayMicroseconds(100);
    }

    vOffset = (max_v + min_v) / 2;
    iOffset = (max_i + min_i) / 2;

    vStep = 34.78 / (max_v - min_v);
    iStep = 0.398 / (max_i - min_i);

    // Serial.println("vOffset: " + String(vOffset));
    // Serial.println("iOffset: " + String(iOffset));
    // Serial.println("max_v: " + String(max_v));
    // Serial.println("min_v: " + String(min_v));
    // Serial.println("max_i: " + String(max_i));
    // Serial.println("min_i: " + String(min_i));

    start = micros();

    // Serial.println("START========================");

    // The voltage signal has a decimal offset of 1885 that corresponds to 0V. Read the voltage pin until the measurement equals to the offset value.
    while(measure > vOffset + 100 || measure < vOffset - 100){
        adc_oneshot_read(adc1_handle, vPin, &measure);

        // Serial.println("Voltage: " + String(measure));

        stop = micros();

        // handle the disconnected load so no watchdog reset is triggered
        // If no desired voltage is read in 100ms, the load is disconnected
        if (stop - start > 100000) {
            Serial.println("Load disconnected");

            for (int i = 0; i < nSamples; i++) {
                powerArrays->vCalibrated[i] = 0;
                powerArrays->cCalibrated[i] = 0;
            }

            powerMes.activePower = 0;
            powerMes.apparentPower = 0;
            powerMes.powerFactor = 0;
            
            return powerMes;
        }
        
    }

    // Serial.println("STOP========================");

    int voltage;
    int current;
    double power;

    // When the voltage value is equal to the offset, start sampling the voltage and current signals.
    for (int i = 0; i < nSamples; i++){
        start = micros();

        adc_oneshot_read(adc1_handle, iPin, &current);
        adc_oneshot_read(adc1_handle, vPin, &voltage);
        
        powerArrays->vCalibrated[i] = (voltage - vOffset) * vStep;
        powerArrays->cCalibrated[i] = (current - iOffset) * iStep;

        power = powerArrays->vCalibrated[i] * powerArrays->cCalibrated[i];
        
        powerSum += power;
        aRmsSum += powerArrays->cCalibrated[i] * powerArrays->cCalibrated[i];
        vRmsSum += powerArrays->vCalibrated[i] * powerArrays->vCalibrated[i];

        stop = micros();
        if(stop - start < time_delay)
            delayMicroseconds(time_delay - (stop - start));

        // rate = analogRead(ratePint);
        // rate = map(rate, 0, 4096, 250, 20000);
    }

    powerArrays->aRms = sqrt(aRmsSum / nSamples);
    powerArrays->vRms = sqrt(vRmsSum / nSamples);

    powerMes.activePower = powerSum / nSamples;
    powerMes.apparentPower = powerArrays->vRms * powerArrays->aRms;

    powerMes.powerFactor = powerMes.activePower / powerMes.apparentPower;

    return powerMes;

}

