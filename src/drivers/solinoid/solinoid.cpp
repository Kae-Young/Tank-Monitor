#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/solinoid/solinoid.h"

#define relay_solenoid 13
#define relay_motor 25
#define SW1 15

void solinoid_setup() {
    gpio_init(SW1);
    gpio_set_dir(SW1, GPIO_IN);
    //gpio_pull_up(SW1);
    gpio_init(relay_solenoid);
    gpio_set_dir(relay_solenoid, GPIO_OUT);
    //gpio_put(relay, 0);
    gpio_init(relay_motor);
    gpio_set_dir(relay_motor, GPIO_OUT);
}

void toggle_relay_solenoid() {
    if (gpio_get(SW1) == 1){
        bool state_solenoid = gpio_get(relay_solenoid);
        gpio_put(relay_solenoid, !state_solenoid);
        bool state_motor = gpio_get(relay_motor);
        gpio_put(relay_motor, !state_motor);
    }
}