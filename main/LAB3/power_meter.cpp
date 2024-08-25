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

    int nSamples = 125000 / time_delay;
    
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

    // Serial.println("Voltage: " + String(measure));

    start = micros();

    // The voltage signal has a decimal offset of 1885 that corresponds to 0V. Read the voltage pin until the measurement equals to the offset value.
    while(measure > 1900 || measure < 1850){
        adc_oneshot_read(adc1_handle, vPin, &measure);
        // measure = analogRead(vPin);

        stop = micros();

        // handle the disconnected load so no watchdog reset is triggered
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

    int voltage;
    int current;
    double power;

    // When the voltage value is equal to the offset, start sampling the voltage and current signals.
    for (int i = 0; i < nSamples; i++){
        start = micros();

        adc_oneshot_read(adc1_handle, iPin, &current);
        adc_oneshot_read(adc1_handle, vPin, &voltage);

        // powerArrays->cArray[i] = analogRead(iPin);
        // powerArrays->vArray[i] = analogRead(vPin);
        
        powerArrays->vCalibrated[i] = (voltage - vOffset) * vStep;
        powerArrays->cCalibrated[i] = (current - iOffset) * iStep;

        power = powerArrays->vCalibrated[i] * powerArrays->cCalibrated[i];
        
        powerSum += power;
        aRmsSum += powerArrays->cCalibrated[i] * powerArrays->cCalibrated[i];
        vRmsSum += powerArrays->vCalibrated[i] * powerArrays->vCalibrated[i];

        stop = micros();

        delayMicroseconds(time_delay - (stop - start));

        // rate = analogRead(ratePint);
        // rate = map(rate, 0, 4096, 250, 20000);
    }

    powerArrays->aRms = sqrt(aRmsSum / nSamples);
    powerArrays->vRms = sqrt(vRmsSum / nSamples);

    powerMes.activePower = powerSum / nSamples;
    powerMes.apparentPower = powerArrays->vRms * powerArrays->aRms;

    powerMes.powerFactor = powerMes.activePower / powerMes.apparentPower;

    for (int i = 0; i < nSamples; i++){
        // Serial.print(4000);
        // Serial.print(",");
        // Serial.print(0);
        // Serial.print(",");
        
        // Serial.print(powerArrays->vArray[i]);
        // Serial.print(",");
        // Serial.print(rate);
        // Serial.print(",");
        // Serial.print(vCalibrated[i]);
        // Serial.print(",");
        // Serial.println(powerArrays->vRms);
        // Serial.print(",");

        // Serial.print(cArray[i]);
        // Serial.print(",");
        // Serial.print(cCalibrated[i]);
        // Serial.print(",");
        // Serial.println(powerArrays->aRms);
        // Serial.print(",");

        // Serial.print(power[i]);
        // Serial.print(",");
        // Serial.print(activePower);
        // Serial.print(",");
        // Serial.print(apparentPower);
        // Serial.print(",");
        // Serial.print(powerFactor);

        // Serial.println();
    }

    // delay(500);

    return powerMes;

}

