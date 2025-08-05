#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/flowrate/flowrate.h"
#include "drivers/ultrasonic/HC-SR04.h"

#define flowrate_sensor_pin 17
#define Pulses_Per_Litre 450.0
#define NUM_SAMPLES 5

volatile double total_water_volume = 0.0;
volatile uint32_t pulse_count = 0;
volatile double flow_readings[NUM_SAMPLES] = {0};
volatile double previous_avg = 0;
volatile double current_avg = 0;
volatile double start_volume = 0;
int sample_count = 0;

//This average function is called upon when calculating the average flow rate over the last 5 readings. It indexes through each array value
//sums them and divides by the count of array values.
volatile double calculate_average(volatile double *buffer, int count) {
    volatile double sum = 0;
    for (int i = 0; i < count; i++) {
        sum += buffer[i];
    }
    return sum / count; 
}

//This function is called upon everytime a pulse is sent from the flow rate sensor it checks for the rising edge of the square wave signal
//Then it indexes total water volume by one pulse which is converted to litres by dividing by the pulses per litre variable from data sheet
//Also adds 1 to pulse count which gets reset every second allowing for flow rate (L/min) to be calculated.
void pulse_handler(uint gpio, uint32_t events) {
    if (gpio == flowrate_sensor_pin && (events & GPIO_IRQ_EDGE_RISE)) {
        total_water_volume += 1.0 / Pulses_Per_Litre;
        pulse_count++;
    }
}
//Initialise flow rate pin and call pulse handler function whenever a pulse is detected from flow rate sensor
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
    //Pass the most recent flow rate calculation into the readings array 
    flow_readings[sample_count % NUM_SAMPLES] = flow_rate;
    sample_count++;
    //This allows the average to be calculated even when the array is not full
    int count = sample_count < NUM_SAMPLES ? sample_count : NUM_SAMPLES;
    //Calculate the average of the last 5 flow rates
    current_avg = calculate_average(flow_readings, count);
    previous_avg = current_avg;
    return flow_rate;
}

double measured_volume()
{
    return start_volume - total_water_volume;
}