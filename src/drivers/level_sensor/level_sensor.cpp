#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "drivers/board.h"
#include "hardware/adc.h"
#include <math.h>

/*  time constant reference table
    T    |   V
    --------------
    0.5  |   39.3%
    0.7  |   50.3%
    1.0  |   63.2%
    2.0  |   86.5%
    3.0  |   95.0%
    4.0  |   98.2%
    5.0  |   99.3%
*/

//electrical paramaters
const float resistance = 1000000;
const float max_discharge_voltage = 0.5;
const float min_charged_voltage = 0.865 * 3.3;
const float vref = 3.3;
const float adc_max = 4095;


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
    //discharge phase
    gpio_put(LVL_SENS_TEST_SIG_PIN, false); //set test pin to 0V

    //wait for system to discharge
    float Vout;;
    do
    {
        uint16_t raw = adc_read();  // Raw ADC reading (0–4095)
        Vout = (raw / adc_max) * vref;
    } while (Vout > max_discharge_voltage);

    //charge up phase
    absolute_time_t charge_start_time = get_absolute_time();    //start clock for charge up phase
    gpio_put(LVL_SENS_TEST_SIG_PIN, true);  //turn test voltage on
    //wait for cap to charge
    do
    {
        uint16_t raw = adc_read();  // Raw ADC reading (0–4095)
        Vout = (raw / adc_max) * vref;
    } while (Vout < min_charged_voltage);
    absolute_time_t charge_end_time = get_absolute_time();  //end clock for charge up

    //final calculations
    float charge_time_us = absolute_time_diff_us(charge_start_time, charge_end_time);
    float capacitance = (charge_time_us*1000000)/(2*resistance);

    return capacitance;
}