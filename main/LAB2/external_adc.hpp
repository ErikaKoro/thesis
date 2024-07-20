#ifndef EXTERNAL_ADC_HPP
#define EXTERNAL_ADC_HPP

#include "../components/arduino/libraries/MCP3XXX/src/MCP3XXX.h"
// #include "MCP3XXX.h"

typedef struct{
    MCP3004 adc;
    int mcp_data;
    int mcp_voltage;
} external_adc_t;

void init_external_adc(external_adc_t *mcp);

void print_external_adc(external_adc_t *mcp);

#endif // EXTERNAL_ADC_HPP