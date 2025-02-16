#include "external_adc.hpp"
#include "../LCD/lcd_espi.hpp"


void init_external_adc(external_adc_t *mcp){

    mcp->adc.begin(15, 23, 19, 18);  // was 15, 13, 12, 14

}

void external_adc_cal(external_adc_t *mcp){

    int extern_sum = 0;
    int extern_avg = 0;

    for(int i = 0; i < 64; i ++){
        extern_sum += mcp->adc.analogRead(0);
        delay(10);
    }
    extern_avg = extern_sum / 64;

    // Calibrate the external average
    if (extern_avg < 265){
        extern_avg += 11;
    
    } else if (extern_avg < 291){
        extern_avg += 12;

    } else if (extern_avg < 360){
        extern_avg += 13;

    } else if (extern_avg < 404){
        extern_avg += 14;

    } else if (extern_avg < 450){
        extern_avg += 15;

    } else if (extern_avg < 497){
        extern_avg += 16;

    } else if (extern_avg < 643){
        extern_avg += 17;

    } else if (extern_avg < 679){
        extern_avg += 18;

    } else if (extern_avg < 762){
        extern_avg += 19;

    } else if (extern_avg < 777){
        extern_avg += 20;

    } else if (extern_avg < 787){
        extern_avg += 19;
        
    } else if (extern_avg < 797){
        extern_avg += 18;
        
    } else if (extern_avg < 809){
        extern_avg += 17;
        
    } else if (extern_avg < 821){
        extern_avg += 16;
        
    } else if (extern_avg < 834){
        extern_avg += 15;
        
    } else if (extern_avg < 850){
        extern_avg += 14;
        
    } else if (extern_avg < 866){
        extern_avg += 13;
        
    } else if (extern_avg < 882){
        extern_avg += 12;
        
    } else if (extern_avg < 898){
        extern_avg += 11;
        
    } else if (extern_avg < 939){
        extern_avg += 10;
        
    } else if (extern_avg < 1014){
        extern_avg += 9;
        
    } else {
        extern_avg = 1023;
    }

    mcp->mcp_data = extern_avg;

    int voltage = (3291.0 / 1024) * extern_avg;
    mcp->mcp_voltage = voltage;

    // update_external_adc(extern_avg, voltage);

    // String adc_value = "ADC value: ";
    // String data = adc_value + String(extern_avg);
    // String voltage_str = "Voltage: ";
    // String voltage_value = voltage_str + String(voltage) + " mV";
    // Serial.println(data);
    // Serial.println(voltage_value);

}