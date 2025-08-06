#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "led.h"
#include "drivers/board.h"

int blink_period = 500000;
bool state = false;
absolute_time_t LED_time_ref;

void LED_init()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_pull_down(LED_PIN);
    LED_time_ref = get_absolute_time();
}

void LED_on()
{
    gpio_put(LED_PIN, true);
}

void LED_off()
{
    gpio_put(LED_PIN, false);
}

void LED_blink()
{
    absolute_time_t current_time = get_absolute_time();
    int time_diff = absolute_time_diff_us(LED_time_ref, current_time);
    gpio_put(LED_PIN, state);
    if (time_diff > 500000 && state)
    {
        LED_time_ref = get_absolute_time();
        state = !state;
    } 
    
}