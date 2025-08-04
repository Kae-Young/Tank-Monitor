#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "drivers/board.h"
#include <math.h>

int timeout = 26100;

void ultrasonic_init()
{
    gpio_init(TRIG_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

float ultrasonic_distance()
{
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    int width = 0;

    absolute_time_t startTime = get_absolute_time();
    while (gpio_get(ECHO_PIN) == 0)
    {
        absolute_time_t current_time = get_absolute_time();
        int elapsed_time = absolute_time_diff_us(startTime, current_time);
        if (elapsed_time > 500000) return 9999;
    }
    startTime = get_absolute_time();
    while (gpio_get(ECHO_PIN) == 1) 
    {
        width++;
        sleep_us(1);
        if (width > timeout) return 9999;
    }
    absolute_time_t endTime = get_absolute_time();
    
    float pulse_length = absolute_time_diff_us(startTime, endTime);
    float distance = pulse_length / 29 / 2;
    return distance;
}

float ultrasonic_volume()
{
    float distance = ultrasonic_distance();
    float water_level = BUCKET_HEIGHT_CM - distance;
    float pi = 3.14159265358979323846;
    float circle_area = pi*BUCKET_RADIUS*BUCKET_RADIUS;
    float volume_cm3 = water_level * circle_area;
    float volume_L = volume_cm3/1000;
    return volume_L;
}