#ifndef WAVES_HPP
#define WAVES_HPP

#include <math.h>
#include <inttypes.h>
#include "driver/dac_continuous.h"


#define CONST_PERIOD_2_PI   6.2832         // 2 * PI

#define N_SAMPLES           400                // Length of wave array
#define DAC_AMPLITUDE       127                // Amplitude of DAC voltage. If it's more than 256 will causes dac_output_voltage() output 0.
#define FREQ                1000 * N_SAMPLES   // Frequency of wave


typedef struct {
    uint8_t sin_wave[N_SAMPLES];           // Used to store sine wave values
    uint8_t tri_wave[N_SAMPLES];           // Used to store triangle wave values
    uint8_t saw_wave[N_SAMPLES];           // Used to store sawtooth wave values
    uint8_t squ_wave[N_SAMPLES];           // Used to store square wave values
    uint8_t zer_wave[N_SAMPLES];           // Used to store zero wave values
} waves;


/**
 * @brief Sets up the DAC to output a wave cyclically
 * 
 * @param cont_handle DAC handle
 * @param wave Struct containing the waveforms
 */
void setup_dac(dac_continuous_handle_t *cont_handle, waves *wave);


/**
 * @brief Starts outputting a wave to the DAC cyclically. 
 * 
 * @param dac_handle DAC handle
 * @param wave Struct containing the waveforms
 * @param wave_type Number representing the type of wave to output (0 = sine, 1 = triangle, 2 = sawtooth, 3 = square, 4 = zero)
 */
void output_wave(dac_continuous_handle_t dac_handle, waves *wave, uint8_t wave_type);

#endif // WAVES_HPP