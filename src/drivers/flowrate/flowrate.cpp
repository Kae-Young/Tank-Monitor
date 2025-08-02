#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/flowrate/flowrate.h"

#define flowrate_sensor_pin 17

volatile double waterFlow = 0.0;

void pulse_handler(uint gpio, uint32_t events) {
    if (gpio == flowrate_sensor_pin && (events & GPIO_IRQ_EDGE_RISE)) {
        waterFlow += 1.0 / 450.0;
    }
}

void waterFlowData ()
{
    gpio_init(flowrate_sensor_pin);
    gpio_set_dir(flowrate_sensor_pin, GPIO_IN);
    gpio_pull_up(flowrate_sensor_pin);

    gpio_set_irq_enabled_with_callback(flowrate_sensor_pin, GPIO_IRQ_EDGE_RISE, true, &pulse_handler);
}
