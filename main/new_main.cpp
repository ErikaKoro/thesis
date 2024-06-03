#include "Arduino.h"

#include "LAB2/esp_adc.hpp"
#include "LAB2/external_adc.hpp"
#include "LAB2/buzzer.hpp"
#include "LAB2/photoresistor.hpp"
#include "LAB1/lab_1.hpp"
#include "DIP_SWITCH/dip_switch.hpp"
#include "LAB3/power_meter.hpp"

extern "C"{
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"
    #include "driver/gpio.h"
}


#define UNIT_1                 ADC_UNIT_1 
#define ADC_ATTEN              ADC_ATTEN_DB_11
#define ESP_INTR_FLAG_DEFAULT  0


// Enumeration that contains all possible modes of the DIP SWITCH
enum mode{
    LAB1 = 0,   // 0 0 on the DIP SWITCH
    LAB2 = 1,   // 0 1 on the DIP SWITCH
    LAB3 = 2,   // 1 0 on the DIP SWITCH
    BUZZER = 3  // 1 1 on the DIP SWITCH
};


// Flag that confirms the current mode of the DIP SWITCH
static enum mode current_mode = LAB1;


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;

    int level_pin_1 = gpio_get_level(DIP_SWITCH_PIN_1);
    int level_pin_2 = gpio_get_level(DIP_SWITCH_PIN_2);

    if(level_pin_1 == 0 && level_pin_2 == 0){
        current_mode = BUZZER;

    }else if(level_pin_1 == 0 && level_pin_2 == 1){
        current_mode = LAB3;

    }else if(level_pin_1 == 1 && level_pin_2 == 0){
        current_mode = LAB2;
    
    }else if(level_pin_1 == 1 && level_pin_2 == 1){
        current_mode = LAB1;
    }
}



extern "C" void app_main(void) {

    initArduino();

    Serial.begin(115200);

    while(!Serial){
        ; // wait for serial port to connect
    }

    // ABOUT DIP SWITCH
    init_dip_switch();

    // ISR handling for the dip switch
    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(DIP_SWITCH_PIN_1, gpio_isr_handler, (void*) DIP_SWITCH_PIN_1);
    gpio_isr_handler_add(DIP_SWITCH_PIN_2, gpio_isr_handler, (void*) DIP_SWITCH_PIN_2);


    // ABOUT ADC
    // init the ADC instance for unit 1 for sampling
    adc_oneshot_unit_handle_t adc1_handle;
    int status = set_oneshot_adc(&adc1_handle, UNIT_1);

    if (status != 0) {
        Serial.println("Error creating ADC instance");
        return;
    }  



    // ABOUT LAB 1
    int lab1_channel_ok = set_oneshot_channel(adc1_handle, LAB_1_PIN, ADC_ATTEN, ADC_BITWIDTH);

    if(lab1_channel_ok != 0){
        Serial.println("Error configuring ADC channel for LAB 1");
        return;
    }

    calibration_t lab1_calibration;

    lab1_calibration = init_calib(
        LAB_1_PIN,
        ADC_ATTEN,
        ADC_UNIT,
        ADC_BITWIDTH
    );
    measurement_t lab1_measurement;



    // ABOUT LAB 2
    // configure the ADC channel
    int channel_ok = set_oneshot_channel(adc1_handle, ADC1_CHAN7, ADC_ATTEN, ADC_BITWIDTH);
    
    if (channel_ok != 0) {
        Serial.println("Error configuring ADC channel for internal ADC");
        return;
    }

    calibration_t calibration;

    calibration = init_calib(
        ADC1_CHAN7,
        ADC_ATTEN,
        ADC_UNIT,
        ADC_BITWIDTH
    );

    // ABOUT EXTERNAL ADC
    // INIT EXTERNAL ADC
    external_adc_t mcp;
    init_external_adc(&mcp);
    measurement_t measurement;



    // ABOUT BUZZER
    // ACTIVATE BUZZER
    notes fur_elise_notes;
    fur_elise_notes = fur_elise();    


    // ABOUT PHOTO RESISTOR
    // INIT PHOTO RESISTOR
    measurement_t photores_measurement;
    // configure the ADC channel for the photoresistor
    int photo_channel_ok = set_oneshot_channel(adc1_handle, PIN36, ADC_ATTEN, ADC_BITWIDTH);

    if (photo_channel_ok != 0) {
        Serial.println("Error configuring ADC channel for photoresistor");
        return;
    }

    calibration_t photo_calibration;

    photo_calibration = init_calib(
        PIN36,
        ADC_ATTEN,
        ADC_UNIT,
        ADC_BITWIDTH
    );


    // ABOUT LAB 3
    powerMes powerMes;
    powerMes.activePower = 0;
    powerMes.apparentPower = 0;
    powerMes.powerFactor = 0;

    Serial.println("Before stack overflow");

    powerArrays powerArrays;
    powerArrays.aRms = 0;
    powerArrays.vRms = 0;

    powerArrays.vCalibrated = (double *)malloc(nSamples * sizeof(double));
    powerArrays.cCalibrated = (double *)malloc(nSamples * sizeof(double));

    Serial.println("After stack overflow");

    // pinMode(vPin, INPUT);
    // pinMode(iPin, INPUT);

    // ADC_CHANNEL_1 of ADC_UNIT_1 is pin 37
    int lab3_channel_voltage = set_oneshot_channel(adc1_handle, ADC_CHANNEL_1, ADC_ATTEN, ADC_BITWIDTH);

    if(lab3_channel_voltage != 0){
        Serial.println("Error configuring ADC voltage channel for LAB 3");
        return;
    }

    // ADC_CHANNEL_2 of ADC_UNIT_1 is pin 38
    int lab3_channel_current = set_oneshot_channel(adc1_handle, ADC_CHANNEL_2, ADC_ATTEN, ADC_BITWIDTH);

    if(lab3_channel_current != 0){
        Serial.println("Error configuring ADC current channel for LAB 3");
        return;
    }

    gpio_isr_handler(0);

    while(true){

        switch(current_mode){

            case LAB1: {

                // LAB 1 measurements
                lab1_measurement = read_calibrate(
                    LAB_1_PIN,
                    adc1_handle,
                    lab1_calibration
                );

                Serial.println("LAB 1 Raw: " + String(lab1_measurement.raw) + " ");
                Serial.println("LAB 1 Voltage: " + String(lab1_measurement.voltage) + " mV");
                Serial.println();
                Serial.println();

            }break;

            case LAB2: {

                // INTERNAL ADC
                measurement = read_calibrate(
                    ADC1_CHAN7,
                    adc1_handle,
                    calibration
                );

                Serial.println("Raw: " + String(measurement.raw) + " ");
                Serial.println("Voltage: " + String(measurement.voltage) + " mV");
                Serial.println();
                Serial.println();


                // EXTERNAL ADC
                Serial.println("EXTERNAL ADC");
                print_external_adc(&mcp);


                // PHOTORESISTOR
                photores_measurement = read_calibrate(
                    PIN36,
                    adc1_handle,
                    photo_calibration
                );
                Serial.println("Photoresistor voltage:" + String(photores_measurement.voltage) + " mV");
                int photo_resistance = volt_to_resistance(photores_measurement.voltage);
                Serial.println("Photoresistor resistance: " + String(photo_resistance) + " ");

            }break;

            case LAB3: {

                Serial.println("LAB 3 mode");

                powerMes = power_meter(&powerArrays, adc1_handle);

                Serial.println("Voltage RMS: " + String(powerArrays.vRms) + " V");
                Serial.println("Current RMS: " + String(powerArrays.aRms) + " A");
                Serial.println("Active Power: " + String(powerMes.activePower) + " W");
                Serial.println("Apparent Power: " + String(powerMes.apparentPower) + " VA");
                Serial.println("Power Factor: " + String(powerMes.powerFactor) + " ");
                Serial.println();

                delay(1000);

            }break;

            case BUZZER: {

                // BUZZER
                setBuzzer(fur_elise_notes);

            }break;

            default: {
                Serial.println("Invalid mode");
            }break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));

    }

    // destructions and free resources
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
    if (calibration.do_cali) {
        adc_calibration_deinit(calibration.cali_handle);
    }
    if(photo_calibration.do_cali){
        adc_calibration_deinit(photo_calibration.cali_handle);
    }
    if(lab1_calibration.do_cali){
        adc_calibration_deinit(lab1_calibration.cali_handle);
    }

    adc_cali_delete_scheme_line_fitting(lab1_calibration.cali_handle);
    adc_cali_delete_scheme_line_fitting(calibration.cali_handle);
    adc_cali_delete_scheme_line_fitting(photo_calibration.cali_handle);

    free(powerArrays.vCalibrated);
    free(powerArrays.cCalibrated);
}