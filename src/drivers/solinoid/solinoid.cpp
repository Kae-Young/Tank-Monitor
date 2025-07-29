#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/solinoid/solinoid.h"

#define relay 13
#define SW1 15

void solinoid_setup() {
    gpio_init(SW1);
    gpio_set_dir(SW1, GPIO_IN);
    //gpio_pull_up(SW1);
    gpio_init(relay);
    gpio_set_dir(relay, GPIO_OUT);
    //gpio_put(relay, 0);
}

void toggle_relay() {
    if (gpio_get(SW1) == 1){
        bool state = gpio_get(relay);
        gpio_put(relay, !state);
    }
}