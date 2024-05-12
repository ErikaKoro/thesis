#ifndef INTERNAL_ADC_HPP
#define INTERNAL_ADC_HPP


extern "C"{
    #include "esp_adc/adc_cali.h"
    #include "esp_adc/adc_oneshot.h"
    #include "sdkconfig.h"
}

#if CONFIG_IDF_TARGET_ESP32
#define ADC1_CHAN7          ADC_CHANNEL_7
#endif

#define ADC_ATTEN           ADC_ATTEN_DB_11
#define ADC_UNIT            ADC_UNIT_1
#define ADC_BITWIDTH        ADC_BITWIDTH_12

typedef struct {

    int adc_raw;
    int voltage;

} measurement_t;


adc_oneshot_unit_handle_t init_oneshot_adc();

void init_oneshot_channel(adc_oneshot_unit_handle_t *handle, adc_channel_t channel);

bool init_cali_handle(adc_channel_t channel, adc_cali_handle_t *cali_handle);

measurement_t read_and_calibrate(adc_channel_t channel, adc_oneshot_unit_handle_t adc1_handle,
                               adc_cali_handle_t cali_handle,
                               bool do_calibration);

void adc_calibration_deinit(adc_cali_handle_t handle);

#endif // INTERNAL_ADC_HPP