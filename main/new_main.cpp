#include "Arduino.h"

#include "LAB2/esp_adc.hpp"
#include "LAB2/external_adc.hpp"
#include "LAB2/buzzer.hpp"
#include "LAB2/photoresistor.hpp"
#include "LAB1/lab_1.hpp"
#include "DIP_SWITCH/dip_switch.hpp"
#include "LAB3/power_meter.hpp"
#include "LCD/lcd_espi.hpp"
#include "LCD/waveform.hpp"


extern "C"{
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h"
    #include "driver/gpio.h"
    
}


#define UNIT_1                 ADC_UNIT_1 
#define ADC_ATTEN              ADC_ATTEN_DB_11
#define ESP_INTR_FLAG_DEFAULT  0



// // Enumeration that contains all possible modes of the DIP SWITCH
// enum mode{
//     LAB1 = 0,   // 0 0 on the DIP SWITCH
//     LAB2 = 1,   // 0 1 on the DIP SWITCH
//     LAB3 = 2,   // 1 0 on the DIP SWITCH
//     BUZZER = 3  // 1 1 on the DIP SWITCH
// };


// The current mode of the DIP SWITCH
static enum mode current_mode = LAB1;

// bool display1 = true;
// bool update1 = true;


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;

    int level_pin_1 = gpio_get_level(DIP_SWITCH_PIN_1);
    int level_pin_2 = gpio_get_level(DIP_SWITCH_PIN_2);
    int level_pin_3 = gpio_get_level(DIP_SWITCH_PIN_3);
    int level_pin_4 = gpio_get_level(DIP_SWITCH_PIN_4);

    if(level_pin_1 == 0 && level_pin_2 == 0 && level_pin_3 == 0 && level_pin_4 == 0){
        current_mode = BUZZER;

    }else if(level_pin_1 == 0 && level_pin_2 == 1 && level_pin_3 == 1 && level_pin_4 == 1){
        current_mode = LAB3;

    }else if(level_pin_1 == 1 && level_pin_2 == 0 && level_pin_3 == 1 && level_pin_4 == 1){
        current_mode = LAB2;
    
    }else if(level_pin_1 == 1 && level_pin_2 == 1 && level_pin_3 == 1 && level_pin_4 == 1){
        current_mode = LAB1;
    }
    else if(level_pin_1 == 0 && level_pin_2 == 0 && level_pin_3 == 1 && level_pin_4 == 1){
        current_mode = PHOTORES;
    }
    else if(level_pin_1 == 1 && level_pin_2 == 0 && level_pin_3 == 0 && level_pin_4 == 1){
        current_mode = WAVEFORM;
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
    gpio_isr_handler_add(DIP_SWITCH_PIN_3, gpio_isr_handler, (void*) DIP_SWITCH_PIN_3);
    gpio_isr_handler_add(DIP_SWITCH_PIN_4, gpio_isr_handler, (void*) DIP_SWITCH_PIN_4);

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
        UNIT_1,
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
        UNIT_1,
        ADC_BITWIDTH
    );

    // ABOUT EXTERNAL ADC
    // INIT EXTERNAL ADC
    external_adc_t mcp;
    mcp.mcp_data = 0;
    mcp.mcp_voltage = 0;
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
        UNIT_1,
        ADC_BITWIDTH
    );


    // ABOUT LAB 3
    powerMes powerMes;
    powerMes.activePower = 0;
    powerMes.apparentPower = 0;
    powerMes.powerFactor = 0;

    powerArrays powerArrays;
    powerArrays.aRms = 0;
    powerArrays.vRms = 0;

    powerArrays.vCalibrated = (double *)malloc(nSamples * sizeof(double));
    powerArrays.cCalibrated = (double *)malloc(nSamples * sizeof(double));

    // pinMode(vPin, INPUT);
    // pinMode(iPin, INPUT);

    // ADC_CHANNEL_1 of ADC_UNIT_1 is pin 37
    int lab3_channel_voltage = set_oneshot_channel(adc1_handle, vPin, ADC_ATTEN, ADC_BITWIDTH);

    if(lab3_channel_voltage != 0){
        Serial.println("Error configuring ADC voltage channel for LAB 3");
        return;
    }

    // ADC_CHANNEL_2 of ADC_UNIT_1 is pin 38
    int lab3_channel_current = set_oneshot_channel(adc1_handle, iPin, ADC_ATTEN, ADC_BITWIDTH);

    if(lab3_channel_current != 0){
        Serial.println("Error configuring ADC current channel for LAB 3");
        return;
    }

    gpio_isr_handler(0);

    boolean first_lab_1 = true;
    boolean first_lab_2 = true;
    boolean first_lab_3 = true;
    boolean first_photores = true;
    boolean first_waveform = true;
    sprites spr_2;

    spr_2.tft.init();
    spr_2.tft.setRotation(1);
    spr_2.tft.fillScreen(TFT_BLACK);

    double test_amp = 17.0;

    while(true){

        switch(current_mode){

            case LAB1: {

                if(first_lab_1 || !first_lab_2 || !first_lab_3 || !first_photores || !first_waveform){
                    
                    if(!first_lab_2){

                        // delete previous sprites
                        spr_2.mcp_vol.deleteSprite();
                        spr_2.mcp.deleteSprite();
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if(!first_lab_3){
                        // delete previous sprites
                        spr_2.rms.deleteSprite();
                        spr_2.active.deleteSprite();
                        spr_2.apparent.deleteSprite();
                        spr_2.factor.deleteSprite();
                    }
                    else if(!first_photores){
                        // delete previous sprites
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if(!first_waveform){
                        // delete previous sprites
                        spr_2.waveform.deleteSprite();
                    }

                    setup_display(&spr_2, "Volt: ", current_mode);
                    init_lab(&spr_2, current_mode, "Lab 1: ");
                    first_photores = true;
                    first_lab_1 = false;
                    first_lab_2 = true;
                    first_lab_3 = true;
                    first_waveform = true;
                }

                // LAB 1 measurements
                lab1_measurement = read_calibrate(
                    LAB_1_PIN,
                    adc1_handle,
                    lab1_calibration
                );

                // fill all sprites with black to erase previous data
                spr_2.raw.fillSprite(TFT_BLACK);
                spr_2.voltage.fillSprite(TFT_BLACK);
                // spr_2.mcp.fillSprite(TFT_BLACK);
                // spr_2.mcp_vol.fillSprite(TFT_BLACK);

                // adc raw and calibrated voltage
                spr_2.raw.drawString(String(lab1_measurement.raw), 2, 0);
                spr_2.voltage.drawString(String(lab1_measurement.voltage) + " mV", 2, 0);

                spr_2.raw.pushSprite(spr_2.start_pixel, 60);
                spr_2.voltage.pushSprite(spr_2.start_pixel, 110);

            }break;

            case LAB2: {

                // Serial.println("Photoresistor voltage:" + String(photores_measurement.voltage) + " mV");
                
                // Serial.println("Photoresistor resistance: " + String(photo_resistance) + " ");

                if(first_lab_2 || !first_lab_1 || !first_lab_3 || !first_photores || !first_waveform){

                    if(!first_lab_3){
                        // delete previous sprites
                        spr_2.rms.deleteSprite();
                        spr_2.active.deleteSprite();
                        spr_2.apparent.deleteSprite();
                        spr_2.factor.deleteSprite();

                    }
                    else if(!first_lab_1){
                        // delete previous sprites
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if(!first_photores){
                        // delete previous sprites
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if(!first_waveform){
                        // delete previous sprites
                        spr_2.waveform.deleteSprite();
                    }

                    setup_display(&spr_2, "MCP Volt: ", current_mode);
                    init_lab(&spr_2, current_mode, "Lab 2: ");
                    first_photores = true;
                    first_lab_2 = false;
                    first_lab_1 = true;
                    first_lab_3 = true;
                    first_waveform = true;
                }

                // INTERNAL ADC
                measurement = read_calibrate(
                    ADC1_CHAN7,
                    adc1_handle,
                    calibration
                );
                
                // fill all sprites with black to erase previous data
                spr_2.raw.fillSprite(TFT_BLACK);
                spr_2.voltage.fillSprite(TFT_BLACK);
                spr_2.mcp.fillSprite(TFT_BLACK);
                spr_2.mcp_vol.fillSprite(TFT_BLACK);

                // adc raw and calibrated voltage
                spr_2.raw.drawString(String(measurement.raw), 2, 0);
                spr_2.voltage.drawString(String(measurement.voltage) + " mV", 2, 0);

                // EXTERNAL ADC
                // update measurements
                print_external_adc(&mcp);

                // MCP data and voltage
                spr_2.mcp.drawString(String(mcp.mcp_data), 2, 0);
                spr_2.mcp_vol.drawString(String(mcp.mcp_voltage) + " mV", 2, 0);

                spr_2.raw.pushSprite(spr_2.start_pixel, 60);
                spr_2.voltage.pushSprite(spr_2.start_pixel, 110);
                spr_2.mcp.pushSprite(spr_2.start_pixel, 160);
                spr_2.mcp_vol.pushSprite(spr_2.start_pixel, 210);

            }break;

            case PHOTORES: {

                if(first_photores || !first_lab_2 || !first_lab_1 || !first_lab_3 || !first_waveform){

                    if (!first_lab_1)
                    {
                        // delete previous sprites
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if (!first_lab_2)
                    {
                        // delete previous sprites
                        spr_2.mcp_vol.deleteSprite();
                        spr_2.mcp.deleteSprite();
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if (!first_lab_3)
                    {
                        // delete previous sprites
                        spr_2.rms.deleteSprite();
                        spr_2.active.deleteSprite();
                        spr_2.apparent.deleteSprite();
                        spr_2.factor.deleteSprite();
                    }
                    else if (!first_waveform)
                    {
                        // delete previous sprites
                        spr_2.waveform.deleteSprite();
                    }
                    
                    setup_display(&spr_2, "volt: ", current_mode);
                    init_lab(&spr_2, current_mode, "Lab2 Photoresistor");
                    first_photores = false;
                    first_lab_2 = true;
                    first_lab_1 = true;
                    first_lab_3 = true;
                    first_waveform = true;
                }

                // PHOTORESISTOR
                photores_measurement = read_calibrate(
                    PIN36,
                    adc1_handle,
                    photo_calibration
                );

                int photo_resistance = volt_to_resistance(photores_measurement.voltage);

                // fill all sprites with black to erase previous data
                spr_2.raw.fillSprite(TFT_BLACK);
                spr_2.voltage.fillSprite(TFT_BLACK);

                spr_2.raw.drawString(String(photores_measurement.raw), 2, 0);
                spr_2.voltage.drawString(String(photores_measurement.voltage) + " mV", 2, 0);

                spr_2.raw.pushSprite(spr_2.start_pixel, 60);
                spr_2.voltage.pushSprite(spr_2.start_pixel, 110);
                    
            }break;

            case LAB3: {

                if(first_lab_3 || !first_lab_2 || !first_lab_1 || !first_photores || !first_waveform){

                    if(!first_lab_2){
                        // delete previous sprites
                        spr_2.mcp_vol.deleteSprite();
                        spr_2.mcp.deleteSprite();
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if(!first_lab_1){
                        // delete previous sprites
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if(!first_photores){
                        // delete previous sprites
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if(!first_waveform){
                        // delete previous sprites
                        spr_2.waveform.deleteSprite();
                    }

                    setup_display_lab3(&spr_2);
                    init_lab(&spr_2, current_mode, "Lab 3: ");
                    first_photores = true;
                    first_lab_3 = false;
                    first_lab_2 = true;
                    first_lab_1 = true;
                    first_waveform = true;
                }

                powerMes = power_meter(&powerArrays, adc1_handle);

                spr_2.rms.fillSprite(TFT_BLACK);
                spr_2.active.fillSprite(TFT_BLACK);
                spr_2.apparent.fillSprite(TFT_BLACK);
                spr_2.factor.fillSprite(TFT_BLACK);

                spr_2.rms.drawString(String(powerArrays.vRms), 0, 0);
                spr_2.active.drawString(String(powerMes.activePower) + "W", 0, 0);
                spr_2.apparent.drawString(String(powerMes.apparentPower) + "VA", 0, 0);
                spr_2.factor.drawString(String(powerMes.powerFactor), 2, 0);

                // Serial.println("Voltage RMS: " + String(powerArrays.vRms) + " V");
                // Serial.println("Current RMS: " + String(powerArrays.aRms) + " A");
                // Serial.println("Active Power: " + String(powerMes.activePower) + " W");
                // Serial.println("Apparent Power: " + String(powerMes.apparentPower) + " VA");
                // Serial.println("Power Factor: " + String(powerMes.powerFactor) + " ");
                // Serial.println();

                spr_2.rms.pushSprite(spr_2.start_pixel, 60);
                spr_2.active.pushSprite(spr_2.start_pixel, 110);
                spr_2.apparent.pushSprite(spr_2.start_pixel, 160);
                spr_2.factor.pushSprite(spr_2.start_pixel, 210);
    

                delay(1000);

            }break;

            case BUZZER: {

                // BUZZER
                setBuzzer(fur_elise_notes);

            }break;

            case WAVEFORM: {

                double max;
                double min;

                if (first_waveform || !first_lab_2 || !first_lab_1 || !first_lab_3 || !first_photores)
                {
                    if (!first_lab_2)
                    {
                        // delete previous sprites
                        spr_2.mcp_vol.deleteSprite();
                        spr_2.mcp.deleteSprite();
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if (!first_lab_1)
                    {
                        // delete previous sprites
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }
                    else if (!first_lab_3)
                    {
                        // delete previous sprites
                        spr_2.rms.deleteSprite();
                        spr_2.active.deleteSprite();
                        spr_2.apparent.deleteSprite();
                        spr_2.factor.deleteSprite();
                    }
                    else if (!first_photores)
                    {
                        // delete previous sprites
                        spr_2.voltage.deleteSprite();
                        spr_2.raw.deleteSprite();
                    }

                    create_sprite_waveform(&spr_2);

                    first_photores = true;
                    first_lab_2 = true;
                    first_lab_1 = true;
                    first_lab_3 = true;
                    first_waveform = false;
                }
                
                double y = 0;
                
                powerMes = power_meter(&powerArrays, adc1_handle);

                display1 = true;
                Graph(&(spr_2.waveform_background), 0, 0, 0, 25, 220, 270, 215, 0, 100, 20, -23, 23, 6, "", "", "", display1, YELLOW);

                
                spr_2.waveform.fillSprite(TFT_BLACK);

                update1 = true;
                for(int i = 0; i < 400; i++){
                    // y = test_amp * sin(0.1 * PI * i * 0.250);

                    Trace(&(spr_2.waveform), i * 0.250, powerArrays.vCalibrated[i], 0, 25, 220, 270, 215, 0, 100, 20, -18, 18, 6, "", "", "", update1, TFT_WHITE);    
                }
                
                // update1 = true;
                // for(int i = 0; i < 400; i++){
                //     y = test_amp * sin(0.1 * PI * i * 0.250 + 0.5) / 2.0;

                //     Trace(&(spr_2.waveform), i * 0.250, y, 0, 25, 220, 270, 215, 0, 100, 20, -18, 18, 6, "", "Time", "", update1, TFT_GREEN);    
                // }

                spr_2.waveform.pushToSprite(&(spr_2.waveform_background), 0, 0, TFT_BLACK);
                spr_2.waveform_background.pushSprite(0, 0);
                
                Serial.println("Waveform mode");            
            }break;

            default: {
                Serial.println("Invalid mode");
            }break;
        }
        
        // vTaskDelay(pdMS_TO_TICKS(1000));

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
