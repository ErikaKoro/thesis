// #include "internal_adc.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "external_adc.hpp"
#include "Arduino.h"
#include "../components/arduino/libraries/MCP3XXX/src/MCP3XXX.h"
// #include "photoresistor.hpp"
#include "buzzer.hpp"
// #include "lab_1.hpp"

extern "C"{
    #include "esp_log.h"
    #include "esp_adc/adc_cali.h"
    #include "esp_adc/adc_oneshot.h"
    #include "sdkconfig.h"
}

extern "C"{
    void app_main(void);
}
#define ADC1_CHAN7          ADC_CHANNEL_7
#define ADC_ATTEN           ADC_ATTEN_DB_11
#define ADC_UNIT            ADC_UNIT_1
#define ADC_BITWIDTH        ADC_BITWIDTH_12
const static char *TAG = "ADC_CALIBRATION";

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
static void adc_calibration_deinit(adc_cali_handle_t handle);



void app_main(void){

    Serial.begin(115200);
    /*
    // create the ADC instance
    // only one handle is needed for all the channels of ADC1
    
    adc_oneshot_unit_handle_t adc_handle = init_oneshot_adc();



    // initialize the calibration handle
    
    adc_cali_handle_t cali_handle = NULL;



    // INTERNAL ADC
    // Init the channel of the ADC instance

    init_oneshot_channel(&adc_handle, ADC1_CHAN7);
    bool do_calibration = init_cali_handle(ADC1_CHAN7, &cali_handle);
    measurement_t measurements;

    // // LAB 1 ADC
    // init_oneshot_channel(&adc_handle, LAB_1_PIN);
    // bool do_calibration_lab_1 = init_cali_handle(LAB_1_PIN, &cali_handle);
    // measurement_t lab_1_measurements;

    // // MCP3004
    // external_adc_t mcp;
    // init_external_adc(&mcp);
    
    // BUZZER
    // notes fur_elise_notes = fur_elise();

    // PHOTO-RESISTOR
    // measurement_t res_measurements;
    init_oneshot_channel(&adc_handle, PIN36);
    // bool do_calibration_photoresistor = init_cali_handle(PIN36, &cali_handle);

    while(1){
        
        // measurements = read_and_calibrate(ADC1_CHAN7, adc_handle, cali_handle, do_calibration);






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
            ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC1_CHAN7, &raw_data));

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









        Serial.println("ADC raw value: " + String(measurements.adc_raw) + " Voltage: " + String(measurements.voltage) + " mV");
        Serial.println(do_calibration ? "Calibration done" : "Calibration not done");

        // photoresistor
        // res_measurements = read_and_calibrate(PIN36, adc_handle, cali_handle, do_calibration_photoresistor);
        // Serial.println("Photoresistor raw value: " + String(res_measurements.adc_raw) + " Voltage: " + String(res_measurements.voltage) + " mV");
        // Serial.println(do_calibration_photoresistor ? "Calibration done" : "Calibration not done");

        // LAB 1
        // lab_1_measurements = read_and_calibrate(LAB_1_PIN, adc_handle, cali_handle, do_calibration_lab_1);
        // Serial.println("LAB 1 raw value: " + String(lab_1_measurements.adc_raw) + " Voltage: " + String(lab_1_measurements.voltage) + " mV");
        // Serial.println(do_calibration_lab_1 ? "Calibration done" : "Calibration not done");

        // int resistance = volt_to_resistance(res_measurements.voltage);

        // setBuzzer(fur_elise_notes);
        
        // Serial.println("External ADC");

        // print_external_adc(&mcp);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    */

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

        int raw_data;
        int cal_voltage;

        for (int i = 0; i < n_samples; i++){
            // read the raw ADC data
            // adc_oneshot_read() returns ESP_OK = 0 on success
            ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN7, &raw_data));
            // ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN0, &adc_raw_2[i]));

            // stabilize the ADC readings by keeping the mean of 10 samples
            sum += raw_data;
            // sum_2 += adc_raw_2[i];

            if(do_calibration){
                // calibrate the raw ADC data
                ESP_ERROR_CHECK(adc_cali_raw_to_voltage(handle, raw_data, &cal_voltage));
                cali_sum += cal_voltage;
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

        Serial.println("ADC raw value: " + String(sum / n_samples) + " Voltage: " + String(cali_sum / n_samples) + " mV");

        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    // destructions and free resources
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));

    if (do_calibration) {
        adc_calibration_deinit(handle);
    }


}


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
