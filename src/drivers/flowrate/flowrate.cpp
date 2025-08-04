#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "drivers/flowrate/flowrate.h"

#define flowrate_sensor_pin 17
#define Pulses_Per_Litre 450.0
#define NUM_SAMPLES 5
#define Max_Flow_Readings 5

volatile double flowTotal = 0.0;

volatile double waterFlow = 0.0;
volatile uint32_t pulse_count = 0;

    //New array code to test
    static double flowRates[Max_Flow_Readings] = {0};   //Create array to store max flow radings

volatile double voltages[NUM_SAMPLES] = {0};
volatile double previous_avg = 0;
volatile double current_avg = 0;
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
        waterFlow += 1.0 / Pulses_Per_Litre;
        pulse_count++;
    }
}

void waterFlowData ()
{
    gpio_init(flowrate_sensor_pin);
    gpio_set_dir(flowrate_sensor_pin, GPIO_IN);
    gpio_pull_up(flowrate_sensor_pin);

    gpio_set_irq_enabled_with_callback(flowrate_sensor_pin, GPIO_IRQ_EDGE_RISE, true, &pulse_handler);
}


// void flow_data ()
// {
//     //pulse_count = 0;
//     double Flow1 = waterFlow;
//     sleep_ms(1000); // wait 1 second

//     // Calculate flow rate in L/min
//     //double flow_rate = (pulse_count / Pulses_Per_Litre) * 60.0;
//     double Flow2 = waterFlow;
//     double flow_rate = (Flow2 - Flow1)*60;

//     //printf("waterFlow: %.3f L\n", waterFlow);
//     printf("Flow rate: %.2f L/min\n", flow_rate);
// }

void flow_data()
{
    pulse_count = 0;
    sleep_ms(1000);
    double flow_rate = (pulse_count / Pulses_Per_Litre) * 60.0;
    voltages[sample_count % NUM_SAMPLES] = flow_rate;
    sample_count++;

     //sleep_ms(500);

    int count = sample_count < NUM_SAMPLES ? sample_count : NUM_SAMPLES;
    current_avg = calculate_average(voltages, count);
    printf("Flowrate: %.2f L/Min  Total Volume %.2f L\n", flow_rate, waterFlow);
    previous_avg = current_avg;
    
}

/*
void flow_data ()
{
    pulse_count = 0;
    sleep_ms(1000); // wait 1 second

    // Calculate flow rate in L/min
    double flow_rate = (pulse_count / Pulses_Per_Litre) * 60.0;
    flowTotal = pulse_count / Pulses_Per_Litre;
    //printf("Flow rate: %.2f L/min  Total Flow: %.2f\n", flow_rate, flowTotal);
    

    flowRates[index] = flow_rate; //set current array value to the sensor reading
    index = (index +1) % Max_Flow_Readings; //set index to next slot until it hits max readings variable

    if(readingsCollected < 5){
        readingsCollected = readingsCollected + 1;
    }
    double sum = 0;
    for(int i = 1; i < readingsCollected; i++){     //loop through entire array of readings and sum all
        sum += flowRates[i];
    }
    double avgFlow = sum / readingsCollected;   //calculate average flow rate over the last 5 seconds
    printf("Average Flow Rate: %.2f L/min  Total Flow: %.2f\n", avgFlow, flowTotal);  //print average flow rate
}
*/
