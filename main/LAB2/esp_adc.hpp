#ifndef ESP_ADC_HPP
#define ESP_ADC_HPP


extern "C"{
    #include "esp_adc/adc_oneshot.h"
    #include "esp_adc/adc_cali.h"
}

#if CONFIG_IDF_TARGET_ESP32
#define ADC1_CHAN7          ADC_CHANNEL_7
#endif

#define ADC_BITWIDTH        ADC_BITWIDTH_12

typedef struct{
    int raw;
    int voltage;
}measurement_t;


typedef struct{
    adc_cali_handle_t cali_handle;
    bool do_cali;
}calibration_t;


int set_oneshot_adc(adc_oneshot_unit_handle_t *adc_handle, adc_unit_t unit);


int set_oneshot_channel(
    adc_oneshot_unit_handle_t adc_handle,
    adc_channel_t channel,
    adc_atten_t atten,
    adc_bitwidth_t bitwidth
);


calibration_t init_calib(
    adc_channel_t channel,
    adc_atten_t atten,
    adc_unit_t unit,
    adc_bitwidth_t bitwidth
);


measurement_t read_calibrate(
    adc_channel_t channel,
    adc_oneshot_unit_handle_t adc1_handle,
    calibration_t calibration
);

static void adc_calibration_deinit(adc_cali_handle_t handle);



#endif // ESP_ADC_HPP