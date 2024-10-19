#include "Arduino.h"
#include "waves.hpp"


/**
 * @brief Generates the waveforms to be outputted to the DAC
 * 
 * @param wave Struct containing the waveforms
 */
void generate_waveforms(waves *wave) {
    
    uint32_t pnt_num = N_SAMPLES;

    for (int i = 0; i < pnt_num; i ++) {
        wave->sin_wave[i] = (uint8_t)((sin(i * CONST_PERIOD_2_PI / pnt_num) + 1) * (double)(DAC_AMPLITUDE) / 2 + 0.5);
        wave->tri_wave[i] = (i > (pnt_num / 2)) ? (2 * DAC_AMPLITUDE * (pnt_num - i) / pnt_num) : (2 * DAC_AMPLITUDE * i / pnt_num);
        wave->saw_wave[i] = (i == pnt_num) ? 0 : (i * DAC_AMPLITUDE / pnt_num);
        wave->squ_wave[i] = (i < (pnt_num / 2)) ? DAC_AMPLITUDE : 0;
        wave->zer_wave[i] = 0;
    }
}


/**
 * @brief Sets up the DAC to output a wave cyclically
 * 
 * @param cont_handle DAC handle
 * @param wave Struct containing the waveforms
 */
void setup_dac(dac_continuous_handle_t *cont_handle, waves *wave) {
    
    // configuration struct for DAC
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_CH0,
        .desc_num = 8,
        .buf_size = 2048,
        .freq_hz = FREQ,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };

    // create new DAC channel in continuous mode
    dac_continuous_new_channels(&cont_cfg, cont_handle);
    
    // enable the DAC channel
    // dac_continuous_enable(*cont_handle);

    // generate waveforms
    generate_waveforms(wave);
}


/**
 * @brief Starts outputting a wave to the DAC cyclically. 
 * 
 * @param dac_handle DAC handle
 * @param wave Struct containing the waveforms
 * @param wave_type Number representing the type of wave to output (0 = sine, 1 = triangle, 2 = sawtooth, 3 = square, 4 = zero)
 */
void output_wave(dac_continuous_handle_t dac_handle, waves *wave, uint8_t wave_type) {
    
    // Based on the wave type, output the corresponding wave. By default, output a zero wave.
    switch (wave_type) {
        case 0:
            dac_continuous_write_cyclically(dac_handle, wave->sin_wave, N_SAMPLES, NULL);
        break;
        
        case 1:
            dac_continuous_write_cyclically(dac_handle, wave->tri_wave, N_SAMPLES, NULL);
        break;
        
        case 2:
            dac_continuous_write_cyclically(dac_handle, wave->saw_wave, N_SAMPLES, NULL);
        break;
        
        case 3:
            dac_continuous_write_cyclically(dac_handle, wave->squ_wave, N_SAMPLES, NULL);
        break;
        
        case 4:
            dac_continuous_write_cyclically(dac_handle, wave->zer_wave, N_SAMPLES, NULL);
        break;
        
        default:
            dac_continuous_write_cyclically(dac_handle, wave->zer_wave, N_SAMPLES, NULL);
        break;
    }
}
