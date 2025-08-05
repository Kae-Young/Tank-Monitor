#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/flowrate/flowrate.h"
#include "drivers/ultrasonic/HC-SR04.h"

#define flowrate_sensor_pin 17
#define Pulses_Per_Litre 450.0
#define NUM_SAMPLES 5
#define Max_Flow_Readings 5

volatile double flowTotal = 0.0;

volatile double total_water_volume = 0.0;
volatile uint32_t pulse_count = 0;

    //New array code to test
    static double flowRates[Max_Flow_Readings] = {0};   //Create array to store max flow radings

volatile double flow_readings[NUM_SAMPLES] = {0};
volatile double previous_avg = 0;
volatile double current_avg = 0;
volatile double start_volume = 0;
int sample_count = 0;

static int index = 0;
static int readingsCollected = 0;   //reset readings back to 0

volatile double calculate_average(volatile double *buffer, int count) {
    volatile double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += buffer[i];
    }
    return sum / count; 
}


void pulse_handler(uint gpio, uint32_t events) {
    if (gpio == flowrate_sensor_pin && (events & GPIO_IRQ_EDGE_RISE)) {
        total_water_volume += 1.0 / Pulses_Per_Litre;
        pulse_count++;
    }
}

void water_flow_init ()
{
    gpio_init(flowrate_sensor_pin);
    gpio_set_dir(flowrate_sensor_pin, GPIO_IN);
    gpio_pull_up(flowrate_sensor_pin);

    equalize_volumes();

    gpio_set_irq_enabled_with_callback(flowrate_sensor_pin, GPIO_IRQ_EDGE_RISE, true, &pulse_handler);
}

void equalize_volumes()
{
    start_volume = ultrasonic_volume();
}

double flow_rate()
{
    pulse_count = 0;
    sleep_ms(1000);
    double flow_rate = (pulse_count / Pulses_Per_Litre) * 60.0;
    flow_readings[sample_count % NUM_SAMPLES] = flow_rate;
    sample_count++;

    int count = sample_count < NUM_SAMPLES ? sample_count : NUM_SAMPLES;
    current_avg = calculate_average(flow_readings, count);
    //printf("Flowrate: %.2f L/Min  Total Volume %.2f L\n", flow_rate, water_volume);
    previous_avg = current_avg;
    return flow_rate;
}

double measured_volume()
{
    return start_volume - total_water_volume;
}
