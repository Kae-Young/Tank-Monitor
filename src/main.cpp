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
#include "drivers/leak/leak.h"


int main()
{
    stdio_init_all();
    buckboost_init();
    init_temperature_sensor();
    stdio_init_all();
    lvl_sens_init();
    ultrasonic_init();
    water_flow_init();
    solinoid_setup();
    static bool solenoid_open_last_cycle = false;
    ui_init();
    absolute_time_t start_time = get_absolute_time();
    
    while (true) {
        // Wait for input
        while (!input_ready) {
            //record sensor readings
            double current_flow_rate = flow_rate();
            double current_measured_volume = measured_volume();
            double current_actual_volume = ultrasonic_volume();
            double current_volume_diff = current_measured_volume - current_actual_volume;
            double current_temperature = read_temperature_celsius();
            absolute_time_t current_time = get_absolute_time();
            double timestamp = absolute_time_diff_us(start_time, current_time);

            //store values and pass into readings print function
            double readings[] = {current_measured_volume, current_actual_volume, current_volume_diff, current_temperature, timestamp};
            print_readings(readings);

            check_for_leak(current_flow_rate, current_measured_volume, current_actual_volume);

            bool solenoid_now = is_solenoid_open();
            if (!solenoid_open_last_cycle && solenoid_now) {
                // It was off, now it's on — so check for blockage
                check_for_blockage();
            }
            solenoid_open_last_cycle = solenoid_now;

            toggle_relay_solenoid();
        }
        input_ready = false;
    }
    return 0;
}