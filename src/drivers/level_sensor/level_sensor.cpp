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
const float vref = 3.3;
const float adc_max = 4095;
const float min_charged_voltage = 0.865 * 3.3;
const float min_charged_raw = 0.865 * adc_max;

//probe dimensions
const float probe_height_mm = 400;
const float probe_outer_rad_mm = 25/2;
const float probe_inner_rad_mm = 16/2;

void lvl_sens_init()
{
    adc_init();

    gpio_init(LVL_SENS_CHARGE_PIN);
    gpio_init(LVL_SENS_DISCHARGE_PIN);
    adc_gpio_init(LVL_SENS_READ_PIN);

    gpio_set_dir(LVL_SENS_CHARGE_PIN, GPIO_OUT);
    gpio_set_dir(LVL_SENS_DISCHARGE_PIN, GPIO_IN);

    gpio_disable_pulls(LVL_SENS_DISCHARGE_PIN);
    gpio_disable_pulls(LVL_SENS_CHARGE_PIN);

    adc_select_input(2);
}

void lvl_sens_discharge()
{
    //discharge phase
    gpio_put(LVL_SENS_CHARGE_PIN, 0); //stop charging capacitor
    gpio_set_dir(LVL_SENS_DISCHARGE_PIN, GPIO_OUT);
    gpio_put(LVL_SENS_DISCHARGE_PIN, 0);    //allows capacitor to discharge

    //wait for system to discharge
    float Vout;;
    do
    {
        uint16_t raw = adc_read();  //raw ADC reading (0–4095)
        Vout = (raw / adc_max) * vref;
        //printf("raw = %i, Vout = %.6f \r\n", raw, Vout);
    } while (Vout > max_discharge_voltage);

    gpio_set_dir(LVL_SENS_DISCHARGE_PIN, GPIO_IN);  //stop capacitor discharging

    //printf("System discharged. Vout = %.6f \r\n", Vout);
}

void lvl_sens_charge()
{
    //absolute_time_t charge_start_time = get_absolute_time();    //start clock for charge up phase
    gpio_put(LVL_SENS_CHARGE_PIN, 1);  //start charging capacitor
    
    //float Vout;
    while (adc_read() < min_charged_raw)
    {
        sleep_us(50);
    }
}

float lvl_sens_read_capacitance()
{
    adc_select_input(2);

    lvl_sens_discharge();

    absolute_time_t charge_start_time = get_absolute_time();    //start clock for charge up phase
    lvl_sens_charge();
    absolute_time_t current_time = get_absolute_time(); //end clock

    //final calculations
    float time_diff_us = absolute_time_diff_us(charge_start_time, current_time);
    //float time_diff_s = time_diff_us/1000000;
    float capacitance = (time_diff_us)/(2*resistance);

    return capacitance;
}