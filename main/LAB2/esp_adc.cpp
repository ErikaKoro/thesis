#include "esp_adc.hpp"
#include "Arduino.h"

extern "C"{
    #include "esp_adc/adc_cali.h"
}

int set_oneshot_adc(adc_oneshot_unit_handle_t *adc_handle, adc_unit_t unit){
    
    // set up the ADC instance with the required configuration
    adc_oneshot_unit_init_cfg_t init_config;
    init_config.unit_id = unit;
    init_config.clk_src = ADC_RTC_CLK_SRC_DEFAULT;

    // create the ADC instance
    int ret = adc_oneshot_new_unit(&init_config, adc_handle);
    if(ret != ESP_OK){
        Serial.println("Error creating ADC instance");
        return -1;
    }
    else{
        Serial.println("ADC instance created");
        return 0;
    }
}


int set_oneshot_channel(
    adc_oneshot_unit_handle_t adc_handle,
    adc_channel_t channel,
    adc_atten_t atten,
    adc_bitwidth_t bitwidth
){
    
    
    adc_oneshot_chan_cfg_t config;
    config.atten = atten;
    config.bitwidth = bitwidth;

    int ret = adc_oneshot_config_channel(adc_handle, channel, &config);
    if(ret != ESP_OK){
        Serial.println("Error configuring ADC channel");
        return -1;
    }
    else{
        Serial.println("ADC channel configured");
        return 0;
    }
}


static bool adc_calibration_init(
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
        // max supported bitwidth is the default(12)
        cali_config.bitwidth = ADC_BITWIDTH;
    
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        Serial.println("Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        Serial.println("eFuse not burnt, skip software calibration");
    } else {
        Serial.println("Invalid arg or no memory");
    }

    return calibrated;
}


calibration_t init_calib(adc_channel_t channel, adc_atten_t atten, adc_unit_t unit, adc_bitwidth_t bitwidth){

    calibration_t calibration;
    calibration.cali_handle = NULL;
    bool do_cali = false;
    do_cali = adc_calibration_init(unit, channel, atten, &calibration.cali_handle);

    adc_cali_line_fitting_config_t scheme_config;
    scheme_config.unit_id = unit;
    scheme_config.atten = atten;
    scheme_config.bitwidth = bitwidth;

    bool scheme_created = adc_cali_create_scheme_line_fitting(&scheme_config, &calibration.cali_handle);

    calibration.do_cali = do_cali;

    return calibration;
}


measurement_t read_calibrate(
    adc_channel_t channel,
    adc_oneshot_unit_handle_t adc1_handle,
    calibration_t calibration
){
    
    int sum = 0;
    int cali_sum = 0;
    measurement_t measurement;

    int raw_data;
    int voltage_value;

    // read the raw value from the ADC
    for(int i = 0; i < 20; i++){
        
        int ret = adc_oneshot_read(adc1_handle, channel, &raw_data);

        if(ret != ESP_OK){
            // Serial.println("Error reading raw value");
        }
        else{
            // Serial.println("Raw value read");
        }
        sum += raw_data;

        if(calibration.do_cali){
            int check_raw_to_volt = adc_cali_raw_to_voltage(calibration.cali_handle, raw_data, &voltage_value);
            cali_sum += voltage_value;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    

    // ESP_ERROR_CHECK(adc_oneshot_read_voltage(adc1_handle, channel, cali_handle, &voltage_value));
    measurement.raw = sum / 20;
    measurement.voltage = cali_sum / 20;

    return measurement;

}


static void adc_calibration_deinit(adc_cali_handle_t handle){

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    // Serial.println("deregister calibration scheme");

    int deleted_scheme = adc_cali_delete_scheme_line_fitting(handle);
    if(deleted_scheme != ESP_OK){
        // Serial.println("Error deleting calibration scheme");
    }
    else{
        // Serial.println("Calibration scheme deleted");
    }
    // ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif

}