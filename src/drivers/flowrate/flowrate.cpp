#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/flowrate/flowrate.h"

#define flowrate_sensor_pin 17
#define Pulses_Per_Litre 450.0

volatile uint32_t pulse_count = 0;

void pulse_handler(uint gpio, uint32_t events) {
    if (gpio == flowrate_sensor_pin && (events & GPIO_IRQ_EDGE_RISE)) {
        pulse_count++;
    }
}

void flow_sensor_init ()
{
    gpio_init(flowrate_sensor_pin);
    gpio_set_dir(flowrate_sensor_pin, GPIO_IN);
    gpio_pull_up(flowrate_sensor_pin);

    gpio_set_irq_enabled_with_callback(flowrate_sensor_pin, GPIO_IRQ_EDGE_RISE, true, &pulse_handler);
}

void flow_data ()
{
    pulse_count = 0;
    sleep_ms(1000); // wait 1 second

    // Calculate flow rate in L/min
    double flow_rate = (pulse_count / Pulses_Per_Litre) * 60.0;

    printf("Flow rate: %.2f L/min\n", flow_rate);
}
