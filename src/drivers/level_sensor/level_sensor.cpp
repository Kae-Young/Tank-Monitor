#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "drivers/board.h"
#include "hardware/adc.h"

const float resistance = 1000000;

//probe dimensions
const float probe_height_mm = 400;
const float probe_outer_rad_mm = 25/2;
const float probe_inner_rad_mm = 16/2;

void level_sensor_init()
{
    adc_init();

    gpio_init(LVL_SENS_TEST_SIG_PIN);
    adc_gpio_init(LVL_SENS_READ_PIN);

    gpio_set_dir(LVL_SENS_TEST_SIG_PIN, GPIO_OUT);
    adc_select_input(1);
}

float level_sensor_read_capacitance()
{
    uint16_t voltage = adc_read();
    
}