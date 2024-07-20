#include "external_adc.hpp"
#include "../LCD/lcd_espi.hpp"


void init_external_adc(external_adc_t *mcp){

    mcp->adc.begin(15, 23, 19, 18);  // was 15, 13, 12, 14

}

void print_external_adc(external_adc_t *mcp){

    int extern_sum = 0;
    int extern_avg = 0;

    for(int i = 0; i < 20; i ++){
        extern_sum += mcp->adc.analogRead(0);
        delay(5);
    }
    extern_avg = extern_sum / 20;
    mcp->mcp_data = extern_avg;

    int voltage = (3291 / 1023) * extern_avg;
    mcp->mcp_voltage = voltage;

    // update_external_adc(extern_avg, voltage);

    // String adc_value = "ADC value: ";
    // String data = adc_value + String(extern_avg);
    // String voltage_str = "Voltage: ";
    // String voltage_value = voltage_str + String(voltage) + " mV";
    // Serial.println(data);
    // Serial.println(voltage_value);

}