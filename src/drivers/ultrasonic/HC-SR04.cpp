#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "drivers/board.h"

int timeout = 26100;

void ultrasonic_init()
{
    gpio_init(TRIG_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

int ultrasonic_distance()
{
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    int width = 0;

    while (gpio_get(ECHO_PIN) == 0) tight_loop_contents();
    absolute_time_t startTime = get_absolute_time();
    while (gpio_get(ECHO_PIN) == 1) 
    {
        width++;
        sleep_us(1);
        if (width > timeout) return 9999;
    }
    absolute_time_t endTime = get_absolute_time();
    
    int pulse_length = absolute_time_diff_us(startTime, endTime);
    return pulse_length / 29 / 2;
}