#include "Arduino.h"

#include "LAB2/esp_adc.hpp"
#include "LAB2/external_adc.hpp"
#include "LAB2/buzzer.hpp"
#include "LAB2/waves.hpp"
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

#define RATE_PIN ADC_CHANNEL_3


// The current mode of the DIP SWITCH
static enum mode current_mode = LAB1;
static enum mode previous_mode = INITIAL;


/**
 * @brief ISR handler for the DIP SWITCH. It reads the state of the DIP SWITCH and sets the current mode accordingly
 */
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;

    int level_pin_1 = gpio_get_level(DIP_SWITCH_PIN_1);
    int level_pin_2 = gpio_get_level(DIP_SWITCH_PIN_2);
    int level_pin_3 = gpio_get_level(DIP_SWITCH_PIN_3);
    int level_pin_4 = gpio_get_level(DIP_SWITCH_PIN_4);

    if(level_pin_1 == 1 && level_pin_2 == 1 && level_pin_3 == 1 && level_pin_4 == 1){
        current_mode = LAB1;

    } else if(level_pin_1 == 1 && level_pin_2 == 1 && level_pin_3 == 1 && level_pin_4 == 0){
        current_mode = LAB2;

    } else if(level_pin_1 == 1 && level_pin_2 == 1 && level_pin_3 == 0 && level_pin_4 == 1){
        current_mode = LAB3;

    } else if(level_pin_1 == 1 && level_pin_2 == 1 && level_pin_3 == 0 && level_pin_4 == 0){
        current_mode = PHOTORES;

    } else if(level_pin_1 == 1 && level_pin_2 == 0 && level_pin_3 == 1 && level_pin_4 == 1){
        current_mode = WAVEFORM;

    } else if(level_pin_1 == 0 && level_pin_2 == 1 && level_pin_3 == 1 && level_pin_4 == 1){
        current_mode = SINE_WAVE;

    } else if(level_pin_1 == 0 && level_pin_2 == 1 && level_pin_3 == 1 && level_pin_4 == 0){
        current_mode = TRIANGLE_WAVE;

    } else if(level_pin_1 == 0 && level_pin_2 == 1 && level_pin_3 == 0 && level_pin_4 == 1){
        current_mode = SAWTOOTH_WAVE;

    } else if(level_pin_1 == 0 && level_pin_2 == 1 && level_pin_3 == 0 && level_pin_4 == 0){
        current_mode = SQUARE_WAVE;
    
    } else{
        current_mode = NOTHING;
    }
}


/**
 * @brief Cleans up the previous lab's sprites and disables the DAC if necessary
 * 
 * @param previous_mode The previous mode of the DIP SWITCH
 * @param tft_sprite The TFT sprite struct
 * @param dac_handle The DAC handle
 */
void lab_cleanup(mode previous_mode, sprites* tft_sprite, dac_continuous_handle_t dac_handle){
    
    switch (previous_mode) {
        case LAB1:
            // delete previous sprites
            tft_sprite->voltage.deleteSprite();
            tft_sprite->raw.deleteSprite();
        break;

        case LAB2:
            // delete previous sprites
            tft_sprite->mcp_vol.deleteSprite();
            tft_sprite->mcp.deleteSprite();
            tft_sprite->voltage.deleteSprite();
            tft_sprite->raw.deleteSprite();
        break;

        case LAB3:
            // delete previous sprites
            tft_sprite->rms.deleteSprite();
            tft_sprite->active.deleteSprite();
            tft_sprite->apparent.deleteSprite();
            tft_sprite->factor.deleteSprite();
        break;

        case PHOTORES:
            // delete previous sprites
            tft_sprite->voltage.deleteSprite();
            tft_sprite->raw.deleteSprite();
        break;
        
        case WAVEFORM:
            // delete previous sprites
            tft_sprite->waveform.deleteSprite();
        break;
        
        case SINE_WAVE:
            dac_continuous_disable(dac_handle);
        break;

        case TRIANGLE_WAVE:
            dac_continuous_disable(dac_handle);
        break;

        case SAWTOOTH_WAVE:
            dac_continuous_disable(dac_handle);
        break;

        case SQUARE_WAVE:
            dac_continuous_disable(dac_handle);
        break;

        default:
        break;
    }
}


extern "C" void app_main(void) {

    initArduino();

    Serial.begin(115200);

    while(!Serial){
        ; // wait for serial port to connect
    }

    // ================== ABOUT DIP SWITCH ==================
    init_dip_switch();

    // ISR handling for the dip switch
    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(DIP_SWITCH_PIN_1, gpio_isr_handler, (void*) DIP_SWITCH_PIN_1);
    gpio_isr_handler_add(DIP_SWITCH_PIN_2, gpio_isr_handler, (void*) DIP_SWITCH_PIN_2);
    gpio_isr_handler_add(DIP_SWITCH_PIN_3, gpio_isr_handler, (void*) DIP_SWITCH_PIN_3);
    gpio_isr_handler_add(DIP_SWITCH_PIN_4, gpio_isr_handler, (void*) DIP_SWITCH_PIN_4);


    // ================== ABOUT ADC ================== 
    
    // init the ADC instance for unit 1 for sampling
    adc_oneshot_unit_handle_t adc1_handle;
    int status = set_oneshot_adc(&adc1_handle, UNIT_1);

    if (status != 0) {
        Serial.println("Error creating ADC instance");
        return;
    }  


    // ================== ABOUT LAB 1 ================== 
   
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


    // ================== ABOUT LAB 2 ==================
    
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


    // ================== ABOUT EXTERNAL ADC ==================

    external_adc_t mcp;
    mcp.mcp_data = 0;
    mcp.mcp_voltage = 0;
    init_external_adc(&mcp);
    measurement_t measurement;

    waves saved_waveforms;
    dac_continuous_handle_t dac_handle;

    // setup the dac and the saved waveforms
    setup_dac(&dac_handle, &saved_waveforms);


    // ================== ABOUT PHOTO RESISTOR ==================

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


    // ================== ABOUT LAB 3 ================== 

    powerMes powerMes;
    powerMes.activePower = 0;
    powerMes.apparentPower = 0;
    powerMes.powerFactor = 0;

    powerArrays powerArrays;
    powerArrays.aRms = 0;
    powerArrays.vRms = 0;
    
    int dynamic_sample_time = 250; // this is the dynamic sample time for the waveform generation
    int dynamic_n_samples = 120000 / dynamic_sample_time; 

    powerArrays.vCalibrated = (double *)malloc(dynamic_n_samples * sizeof(double));
    powerArrays.cCalibrated = (double *)malloc(dynamic_n_samples * sizeof(double));


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


    // ================== ABOUT SAMPLE RATE ==================

    int rate_pin_ok = set_oneshot_channel(adc1_handle, RATE_PIN, ADC_ATTEN, ADC_BITWIDTH);

    if(rate_pin_ok != 0){
        Serial.println("Error configuring ADC channel for rate pin");
        return;
    }


    gpio_isr_handler(0);

    sprites tft_sprite;

    tft_sprite.tft.init();
    tft_sprite.tft.setRotation(1);
    tft_sprite.tft.fillScreen(TFT_BLACK);

    // Serial.println("Hello GIF 1");

    // for (int i = 0; i < 10; i++){
    //     display_hello_gif(tft_sprite.tft);
    // }

    // Serial.println("Hello GIF 8");

    tft_sprite.tft.fillScreen(TFT_BLACK);

    while(true){
        switch (current_mode) {
            case LAB1:
                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Lab 1 mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    setup_display(&tft_sprite, "Volt: ", current_mode);
                    init_lab(&tft_sprite, current_mode, "Lab 1: ");

                    previous_mode = current_mode;
                }

                // LAB 1 measurements
                lab1_measurement = read_calibrate(
                    LAB_1_PIN,
                    adc1_handle,
                    lab1_calibration
                );

                // fill all sprites with black to erase previous data
                tft_sprite.raw.fillSprite(TFT_BLACK);
                tft_sprite.voltage.fillSprite(TFT_BLACK);

                // adc raw and calibrated voltage
                tft_sprite.raw.drawString(String(lab1_measurement.raw), 2, 0);
                tft_sprite.voltage.drawString(String(lab1_measurement.voltage) + " mV", 2, 0);

                tft_sprite.raw.pushSprite(tft_sprite.start_pixel, 60);
                tft_sprite.voltage.pushSprite(tft_sprite.start_pixel, 110);
            break;

            case LAB2:
                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Lab 2 mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    setup_display(&tft_sprite, "MCP Volt: ", current_mode);
                    init_lab(&tft_sprite, current_mode, "Lab 2: ");

                    previous_mode = current_mode;
                }

                // INTERNAL ADC
                measurement = read_calibrate(
                    ADC1_CHAN7,
                    adc1_handle,
                    calibration
                );
                
                // fill all sprites with black to erase previous data
                tft_sprite.raw.fillSprite(TFT_BLACK);
                tft_sprite.voltage.fillSprite(TFT_BLACK);
                tft_sprite.mcp.fillSprite(TFT_BLACK);
                tft_sprite.mcp_vol.fillSprite(TFT_BLACK);

                // adc raw and calibrated voltage
                tft_sprite.raw.drawString(String(measurement.raw), 2, 0);
                tft_sprite.voltage.drawString(String(measurement.voltage) + " mV", 2, 0);

                // EXTERNAL ADC
                // update measurements
                external_adc_cal(&mcp);

                // MCP data and voltage
                tft_sprite.mcp.drawString(String(mcp.mcp_data), 2, 0);
                tft_sprite.mcp_vol.drawString(String(mcp.mcp_voltage) + " mV", 2, 0);

                tft_sprite.raw.pushSprite(tft_sprite.start_pixel, 60);
                tft_sprite.voltage.pushSprite(tft_sprite.start_pixel, 110);
                tft_sprite.mcp.pushSprite(tft_sprite.start_pixel, 160);
                tft_sprite.mcp_vol.pushSprite(tft_sprite.start_pixel, 210);
            break;

            case LAB3:
                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Lab 3 mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    setup_display_lab3(&tft_sprite);
                    init_lab(&tft_sprite, current_mode, "Lab 3: ");

                    previous_mode = current_mode;
                }

                powerMes = power_meter(&powerArrays, adc1_handle, 250);

                tft_sprite.rms.fillSprite(TFT_BLACK);
                tft_sprite.active.fillSprite(TFT_BLACK);
                tft_sprite.apparent.fillSprite(TFT_BLACK);
                tft_sprite.factor.fillSprite(TFT_BLACK);

                tft_sprite.rms.drawString(String(powerArrays.vRms) + ",", 0, 0);
                tft_sprite.rms.drawString(String(powerArrays.aRms), 130, 0);
                tft_sprite.active.drawString(String(powerMes.activePower) + "W", 0, 0);
                tft_sprite.apparent.drawString(String(powerMes.apparentPower) + "VA", 0, 0);
                tft_sprite.factor.drawString(String(powerMes.powerFactor), 2, 0);

                // Serial.println("Voltage RMS: " + String(powerArrays.vRms) + " V");
                // Serial.println("Current RMS: " + String(powerArrays.aRms) + " A");
                // Serial.println("Active Power: " + String(powerMes.activePower) + " W");
                // Serial.println("Apparent Power: " + String(powerMes.apparentPower) + " VA");
                // Serial.println("Power Factor: " + String(powerMes.powerFactor) + " ");
                // Serial.println();

                tft_sprite.rms.pushSprite(tft_sprite.start_pixel - 100, 60);
                tft_sprite.active.pushSprite(tft_sprite.start_pixel, 110);
                tft_sprite.apparent.pushSprite(tft_sprite.start_pixel, 160);
                tft_sprite.factor.pushSprite(tft_sprite.start_pixel, 210);
    

                delay(1000);
            break;

            case PHOTORES:
                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Photoresistor mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    setup_display(&tft_sprite, "volt: ", current_mode);
                    init_lab(&tft_sprite, current_mode, "Lab2 Photoresistor");

                    previous_mode = current_mode;
                }

                // PHOTORESISTOR
                photores_measurement = read_calibrate(
                    PIN36,
                    adc1_handle,
                    photo_calibration
                );

                // int photo_resistance = volt_to_resistance(photores_measurement.voltage);

                // fill all sprites with black to erase previous data
                tft_sprite.raw.fillSprite(TFT_BLACK);
                tft_sprite.voltage.fillSprite(TFT_BLACK);

                tft_sprite.raw.drawString(String(photores_measurement.raw), 2, 0);
                tft_sprite.voltage.drawString(String(photores_measurement.voltage) + " mV", 2, 0);

                tft_sprite.raw.pushSprite(tft_sprite.start_pixel, 60);
                tft_sprite.voltage.pushSprite(tft_sprite.start_pixel, 110);
            break; 

            case SINE_WAVE:

                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Sine wave mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    init_lab(&tft_sprite, current_mode, "DAC: ");

                    dac_continuous_enable(dac_handle);
                    output_wave(dac_handle, &saved_waveforms, 0);

                    previous_mode = current_mode;
                }

            break;

            case TRIANGLE_WAVE:

                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Triangle wave mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    init_lab(&tft_sprite, current_mode, "DAC: ");

                    dac_continuous_enable(dac_handle);
                    output_wave(dac_handle, &saved_waveforms, 1);

                    previous_mode = current_mode;
                }

            break;

            case SAWTOOTH_WAVE:

                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Sawtooth wave mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    init_lab(&tft_sprite, current_mode, "DAC: ");

                    dac_continuous_enable(dac_handle);
                    output_wave(dac_handle, &saved_waveforms, 2);

                    previous_mode = current_mode;
                }

            break;

            case SQUARE_WAVE:

                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Square wave mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    init_lab(&tft_sprite, current_mode, "DAC: ");

                    dac_continuous_enable(dac_handle);
                    output_wave(dac_handle, &saved_waveforms, 3);

                    previous_mode = current_mode;
                }

            break;

            case WAVEFORM: {

                double max;
                double min;

                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("Waveform mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    create_sprite_waveform(&tft_sprite);

                    previous_mode = current_mode;
                }
                
                double y = 0;
                
                powerMes = power_meter(&powerArrays, adc1_handle, dynamic_sample_time);

                vTaskDelay(pdMS_TO_TICKS(5)); // 5ms delay to reset the watchdog timer

                display1 = true;
                // Creates grid and labels for the graph
                Graph(
                    &(tft_sprite.waveform_background), 0, 0, 0,
                    MARGIN_X, MARGIN_Y, WIDTH, HEIGHT,
                    0, 100, 10, -20, 20, 5, "", "",
                    "Volt", "Amp", display1, YELLOW
                );

                
                tft_sprite.waveform.fillSprite(TFT_BLACK);

                // The power array is filled with the voltage values. Sometimes the values start from the negative half of the sine wave and the graph is not displayed correctly. If this 
                // happens, we plot the graph from 40 to 440 and the graph is displayed correctly.
                
                // 5000 us (20000 / 4) is the first time that we reach a maximum or a minimum value depending on the rising or falling edge of the signal.
                // first max gives the index of the first maximum or minimum value in the array
                int first_max = 5000 / dynamic_sample_time;

                // We check the maximum value(positive or negative) to be more precise for its sign
                // earlier values might not be so precise
                if(powerArrays.vCalibrated[first_max] < 0){

                    Serial.println("Negative half of the sine wave");

                    update1 = true;

                    // When the signal starts from the negative half of the sine wave, we plot the graph from index 40 to 440 and neglect the first 40 samples
                    for(int i = 2 * first_max; i < 100000 / dynamic_sample_time + 2 * first_max; i++){

                        Trace(
                            &(tft_sprite.waveform), (i - 2 * first_max) * dynamic_sample_time / 1000.0,
                            powerArrays.vCalibrated[i], 0,
                            MARGIN_X, MARGIN_Y, WIDTH, HEIGHT,
                            0, 100, 10, -20, 20, 5, "", "", "Volt", "Amp", update1, TFT_RED
                        );    
                    }
                
                    update1 = true;
                    
                    for(int i = 2 * first_max; i < 100000 / dynamic_sample_time + 2 * first_max; i++){

                        // 
                        Trace(
                            &(tft_sprite.waveform), (i - 2 * first_max) * dynamic_sample_time / 1000.0,
                            powerArrays.cCalibrated[i] * 20, 0,
                            MARGIN_X, MARGIN_Y, WIDTH, HEIGHT,
                            0, 100, 10, -20, 20, 5, "", "", "Volt", "Amp", update1, TFT_GREEN
                        );    
                    }

                } else {

                    Serial.println("Positive half of the sine wave");

                    update1 = true;
                    // plot the same number of samples with the first case
                    // When the signal starts from the positive half of the sine wave, we plot the graph from index 0 and neglect the last 40 samples
                    // if more samples are plotted, the graph is not displayed correctly.
                    for(int i = 0; i < 100000 / dynamic_sample_time; i++){  //  - 2 * first_max

                        Trace(
                            &(tft_sprite.waveform), i * dynamic_sample_time / 1000.0, powerArrays.vCalibrated[i], 0,
                            MARGIN_X, MARGIN_Y, WIDTH, HEIGHT,
                            0, 100, 10, -20, 20, 5, "", "", "Volt", "Amp", update1, TFT_RED
                        );    
                    }

                    update1 = true;
                    for(int i = 0; i < 100000 / dynamic_sample_time; i++){  //  - 2 * first_max

                        Trace(
                            &(tft_sprite.waveform), i * dynamic_sample_time / 1000.0, powerArrays.cCalibrated[i] * 20, 0,
                            MARGIN_X, MARGIN_Y, WIDTH, HEIGHT,
                            0, 100, 10, -20, 20, 5, "", "", "Volt", "Amp", update1, TFT_GREEN
                        );
                    }

                }

                

                tft_sprite.waveform.pushToSprite(&(tft_sprite.waveform_background), 0, 0, TFT_BLACK);
                tft_sprite.waveform_background.pushSprite(0, 0);
                
                // update the dynamic sample time from the potentiometer
                dynamic_sample_time = read_calibrate(
                    RATE_PIN,
                    adc1_handle,
                    photo_calibration
                ).raw;

                // the range of the sampling period is 250 to 8000. For this reason we must map the value to the range 250 to 8000(sampling period)
                dynamic_sample_time = map(dynamic_sample_time, 0, 4095, 250, 8000);

                dynamic_n_samples = 120000 / dynamic_sample_time;
                

                Serial.println("Sample time: " + String(dynamic_sample_time) + " μs");
                Serial.println("Sample rate: " + String(1000000 / dynamic_sample_time) + " Hz");

                Serial.println();
                Serial.println();
            }break;

            case NOTHING:
                if (current_mode != previous_mode){
                    // This is the first time we enter this mode
                    
                    Serial.println("INVALID mode");

                    lab_cleanup(previous_mode, &tft_sprite, dac_handle);

                    init_lab(&tft_sprite, current_mode, " ");

                    previous_mode = current_mode;
                }
            
            break;

            default:
                // Serial.println("Invalid mode");
            break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(5));  // 5ms delay to reset the watchdog timer
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


