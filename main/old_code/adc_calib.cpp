#include "Arduino.h"
#include "MCP3XXX.h"

extern "C" {
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
}

#if CONFIG_IDF_TARGET_ESP32
#define ADC1_CHAN7          ADC_CHANNEL_7
#define ADC1_CHAN0          ADC_CHANNEL_0
#endif

#define ADC_ATTEN           ADC_ATTEN_DB_11

// typedef struct{
//     MCP3004 adc;
// } external_adc_t;

static int adc_raw[10];
// static int adc_raw_2[10];
static int voltage[10];
// static int voltage_2[10];
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);

// static void set_external_adc();

const static char *TAG = "ADC_CALIBRATION";

extern "C"
{
    void app_main(void);
}

void app_main(void){

    // ADC1 handle represents the ADC1 instance
    adc_oneshot_unit_handle_t adc1_handle;

    // set up the ADC instance with the required configuration
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };

    // create the ADC instance
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // configure the ADC channel
    // adc_oneshot_chan_cfg_t config = {
    //     .atten = ADC_ATTEN,
    //     .bitwidth = ADC_BITWIDTH_12,
    // };

    adc_oneshot_chan_cfg_t config;
    config.atten = ADC_ATTEN;
    config.bitwidth = ADC_BITWIDTH_12;

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHAN7, &config));
    // ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHAN0, &config));


    // initialize the calibration handle
    adc_cali_handle_t handle = NULL;
    bool do_calibration = adc_calibration_init(ADC_UNIT_1, ADC1_CHAN7, ADC_ATTEN, &handle);
    // bool do_calibration_2 = adc_calibration_init(ADC_UNIT_1, ADC1_CHAN0, ADC_ATTEN, &handle);

    // esp32 supports ADC calibration Line fitting scheme
    // To create this scheme, set up the configurations
    // adc_cali_line_fitting_config_t scheme_config = {
    //     .unit_id = ADC_UNIT_1,
    //     .atten = ADC_ATTEN,
    //     .bitwidth = ADC_BITWIDTH_9,
    // };

    adc_cali_line_fitting_config_t scheme_config;
    scheme_config.unit_id = ADC_UNIT_1;
    scheme_config.atten = ADC_ATTEN;
    scheme_config.bitwidth = ADC_BITWIDTH_12;

    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&scheme_config, &handle));

    while(1){
        int sum = 0;
        // int sum_2 = 0;
        int cali_sum = 0;
        // int cali_sum_2 = 0;
        int n_samples = 20;

        for (int i = 0; i < n_samples; i++){
            // read the raw ADC data
            // adc_oneshot_read() returns ESP_OK = 0 on success
            ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN7, &adc_raw[i]));
            // ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN0, &adc_raw_2[i]));

            // stabilize the ADC readings by keeping the mean of 10 samples
            sum += adc_raw[i];
            // sum_2 += adc_raw_2[i];

            if(do_calibration){
                // calibrate the raw ADC data
                ESP_ERROR_CHECK(adc_cali_raw_to_voltage(handle, adc_raw[i], voltage + i));
                cali_sum += voltage[i];
                // ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN5, voltage[i]);
            }

            // if(do_calibration_2){
            //     // calibrate the raw ADC data
            //     ESP_ERROR_CHECK(adc_cali_raw_to_voltage(handle, adc_raw_2[i], voltage_2 + i));
            //     cali_sum_2 += voltage_2[i];
            //     // ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN5, voltage[i]);
            // }

            // convert milliseconds to clock ticks and wait for that time
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        // set_external_adc();

        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC1_CHAN7, sum / n_samples);
        ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN7, cali_sum / n_samples);

        // ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data of photoresistor: %d", ADC_UNIT_1 + 1, ADC1_CHAN7, sum_2 / n_samples);
        // ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage of photoresistor: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN7, cali_sum_2 / n_samples);

        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    // destructions and free resources
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    if (do_calibration) {
        adc_calibration_deinit(handle);
    }
    // if (do_calibration_2) {
    //     adc_calibration_deinit(handle);
    // }
    ESP_LOGI(TAG, "delete %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));

}

// void set_external_adc(){
//     mcp.begin(15, 13, 12, 14);

//     int extern_sum = 0;
//     int extern_avg = 0;

//     for(int i = 0; i < 20; i ++){
//         extern_sum += mcp.adc.analogRead(0);
//         delay(5);
//     }
//     extern_avg = extern_sum / 20;

//     int voltage = (3291 / 1023) * extern_avg;
    
//     String adc_value = "ADC value: ";
//     String data = adc_value + String(extern_avg);
//     String voltage_str = "Voltage: ";
//     String voltage_value = voltage_str + String(voltage) + " mV";
//     Serial.println(data);
//     Serial.println(voltage_value);
// }


/**
 * @param unit ADC unit that indicates the type of ADC (ADC1 or ADC2)
 * @param channel ADC channel (ADC has 8 channels per each unit) 
 * @param atten Attenuation level defines the range of voltage that ADC can measure
 * @param out_handle Pointer to the handle that will be used for calibration
*/
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
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");

        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            // max supported bitwidth is the default(12)
            .bitwidth = ADC_BITWIDTH_9,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

static void adc_calibration_deinit(adc_cali_handle_t handle){

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif

}