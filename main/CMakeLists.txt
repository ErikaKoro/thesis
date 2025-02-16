# idf_component_register(SRCS "new_main.cpp"
#                     INCLUDE_DIRS ".")

# idf_component_register(SRCS
#                     INCLUDE_DIRS ".")

# idf_component_register(SRCS "main.cpp" "internal_adc.cpp" "external_adc.cpp" "photoresistor.cpp" "buzzer.cpp" "lab_1.cpp"
#                     INCLUDE_DIRS ".")


# idf_component_register(SRCS "main.cpp" "internal_adc.cpp"
#                     INCLUDE_DIRS ".")

# idf_component_register(SRCS "new_main.cpp"
#                     INCLUDE_DIRS ".")

idf_component_register(SRCS "new_main.cpp"
                            "LAB2/esp_adc.cpp"
                            "LAB2/external_adc.cpp"
                            "LAB2/waves.cpp"
                            "LAB2/photoresistor.cpp"
                            "LAB1/lab_1.cpp"
                            "DIP_SWITCH/dip_switch.cpp"
                            "LAB3/power_meter.cpp"
                            "LCD/lcd_espi.cpp"
                            "LCD/waveform.cpp"
                         #    "LCD/gif.cpp"
                       INCLUDE_DIRS ".")

# idf_component_register(SRCS "lcd_main.cpp"
#                        INCLUDE_DIRS ".")

