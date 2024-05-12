#include "internal_adc.hpp"
#include "Arduino.h"

extern "C" {
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
}


/**
 * @brief Initialize the ADC instance
 * @return adc_oneshot_unit_handle_t Handle to the ADC instance
*/
adc_oneshot_unit_handle_t init_oneshot_adc(){
    
    // ADC1 handle represents the ADC1 instance
    adc_oneshot_unit_handle_t adc1_handle;

    // set up the ADC instance with the required configuration
    adc_oneshot_unit_init_cfg_t init_config1;
    adc_oneshot_clk_src_t clk_src = ADC_RTC_CLK_SRC_DEFAULT;
    init_config1.unit_id = ADC_UNIT;
    init_config1.clk_src = clk_src;
    

    // create the ADC instance
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    return adc1_handle;
}


/**
 * @brief Initialize the ADC channel
 * @param handle: Handle to the ADC instance.
 * @param channel: ADC channel to be initialized. Each ADC unit has 8 channels.
*/
void init_oneshot_channel(adc_oneshot_unit_handle_t *handle, adc_channel_t channel){

    adc_oneshot_chan_cfg_t config;
    config.atten = ADC_ATTEN;
    config.bitwidth = ADC_BITWIDTH;

    ESP_ERROR_CHECK(adc_oneshot_config_channel(*handle, channel, &config));
    
}


/**
 * @param unit ADC unit that indicates the type of ADC (ADC1 or ADC2)
 * @param channel ADC channel (ADC has 8 channels per each unit) 
 * @param atten Attenuation level defines the range of voltage that ADC can measure
 * @param out_handle Pointer to the handle that will be used for calibration
*/
bool adc_calibration_init(
    adc_unit_t unit,
    adc_channel_t channel,
    adc_atten_t atten,
    adc_cali_handle_t *out_handle
){

    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

// esp32 supports line fitting calibration scheme
#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {

        adc_cali_line_fitting_config_t cali_config;

        cali_config.unit_id = unit;
        cali_config.atten = atten;
        cali_config.bitwidth = ADC_BITWIDTH;

        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        // ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        // ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        // ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}


/**
 * @brief Initialize the calibration handle
 * @param channel: ADC channel to be calibrated
*/
bool init_cali_handle(adc_channel_t channel, adc_cali_handle_t *cali_handle){

    bool do_calibration = adc_calibration_init(ADC_UNIT, channel, ADC_ATTEN, cali_handle);

    adc_cali_line_fitting_config_t scheme_config;
    scheme_config.unit_id = ADC_UNIT;
    scheme_config.atten = ADC_ATTEN;
    scheme_config.bitwidth = ADC_BITWIDTH;

    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&scheme_config, cali_handle));

    return do_calibration;
}


measurement_t read_and_calibrate(adc_channel_t channel, adc_oneshot_unit_handle_t adc1_handle,
                               adc_cali_handle_t cali_handle,
                               bool do_calibration){

    measurement_t measurements;
    int raw_data;
    int cal_voltage;

    int sum = 0;
    int cali_sum = 0;
    int n_samples = 20;

    for (int i = 0; i < n_samples; i++){

        Serial.println("Reading sample " + String(i));
        // read the raw ADC data
        // adc_oneshot_read() returns ESP_OK = 0 on success
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, channel, &raw_data));

        // stabilize the ADC readings by keeping the mean of 10 samples
        sum += raw_data;

        if(do_calibration){

            // calibrate the raw ADC data
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, raw_data, &cal_voltage));

            cali_sum += cal_voltage;
            // ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN5, voltage[i]);
        }

        // convert milliseconds to clock ticks and wait for that time
        vTaskDelay(pdMS_TO_TICKS(5));
    }

        measurements.adc_raw = sum / n_samples;
        measurements.voltage = cali_sum / n_samples;

        // ESP_LOGI("ADC Channel[%d] Raw Data: %d");
        // ESP_LOGI("ADC Channel[%d] Cali Voltage: %d mV");

        return measurements;

}


/**
 * @brief Deregister calibration scheme
 * @param handle: Handle to the calibration scheme
*/
void adc_calibration_deinit(adc_cali_handle_t handle){

// esp32 supports line fitting calibration scheme
#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    // ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif

}