#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/led/led.h"
#include "drivers/board.h"
//#include "drivers/accelerometer/accelerometer.h"
#include "drivers/buckboost/buckboost.h"
#include "drivers/temperature/temp.h"
#include "drivers/level_sensor/level_sensor.h"
#include "drivers/flowrate/flowrate.h"
#include "drivers/solinoid/solinoid.h"
#include "drivers/ultrasonic/HC-SR04.h"
#include "drivers/uart/uart.h"


int main()
{
    stdio_init_all();

    led_blink();
    buckboost_init();
    init_temperature_sensor();
    //init_temperature_sensor();

    uint32_t* led_data = led_init();
    led_set(1, led_data, led_colour(blue));
    led_write(led_data);

    stdio_init_all();
    
    lvl_sens_init();

    ultrasonic_init();

    water_flow_init();

    solinoid_setup();

    ui_init();

    //start clock
    absolute_time_t start_time = get_absolute_time();
    
    while (true) {
        // Wait for input
        while (!input_ready) {
            __asm("wfi");  // Wait for interrupt
        }
        //float temp = read_temperature_celsius();
        //printf("Temperature: %.2f °C\n", temp);
        //sleep_ms(2000);
        //float temp = read_temperature_celsius();
        //printf("Temperature: %.2f °C\n", temp);

        //printf("%.6f \r\n", level_sensor_read_capacitance());

        //printf("waterFlow: %.3f L\n", waterFlow);
        //toggle_relay();
    
        toggle_relay_solenoid();
        //absolute_time_t current_time = get_absolute_time();
        //float timestamp = absolute_time_diff_us(start_time, current_time);
        //printf("Timestamp: %f, Flow rate: %.2f L/min, Measured volume: %.2f L, Actual Volume: %.2f L, Temperature: %.2f \n", timestamp, flow_rate(), measured_volume(), ultrasonic_volume(), read_temperature_celsius());
        //sleep_ms(1000);
    }

    input_ready = false;
    return 0;
}