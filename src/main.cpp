#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include <string.h>

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
    LED_init();
    static bool solenoid_open_last_cycle = false;
    ui_init();

    //store menu options
    char option_menu[20] = "menu";
    char option_read[20] = "read";
    char option_reset[20] = "reset";
    char option_valve[20] = "valve";
    char option_pump[20] = "pump";
    char option_resize[20] = "resize";

    bool is_readings_mode = false;

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
            if (is_readings_mode)
            {
                print_readings(readings);
            }

            check_for_leak(current_flow_rate, current_measured_volume, current_actual_volume);

            bool solenoid_now = is_solenoid_open();
            if (!solenoid_open_last_cycle && solenoid_now) {
                // It was off, now it's on — so check for blockage
                //check_for_blockage();
            }
            solenoid_open_last_cycle = solenoid_now;

            check_for_blockage();

            toggle_relay_solenoid();            
        }
        // INPUT HANDLING
        // Process input
            
        char command[20] = "\000";
        char argument[20] = "\000";
        int input = sscanf(buffer, "%s %[^\t\n]", &command, &argument);
            
        // MENU
        if (strcmp(command, option_menu) == 0)
        {
            is_readings_mode = false;
            draw_ui(is_readings_mode);
            print_output("Ready for command...");
        }
        // READ
        else if (strcmp(command, option_read) == 0)
        {
            is_readings_mode = true;
            draw_ui(is_readings_mode);
            print_output("Ready for command...");
        }
        // RESET
        else if (strcmp(command, option_reset) == 0)
        {
            equalize_volumes();
            print_output("Set expected volume to actual volume.");
        }
        // VALVE
        else if (strcmp(command, option_valve) == 0)
        {
            if (strcmp(argument, "open") == 0)
            {
                set_relay_solenoid(true);
                print_output("Solenoid valve opened.");
            }
            else if (strcmp(argument, "close") == 0)
            {
                set_relay_solenoid(false);
                print_output("Solenoid valve closed.");
            }
            else
            {
                print_output("Invalid argument. Syntax: 'valve [open/close]'.");
            }
        }
        // PUMP
        else if (strcmp(command, option_pump) == 0)
        {
            if (strcmp(argument, "on") == 0)
            {
                set_relay_pump(true);
                print_output("Pump on.");
            }
            else if (strcmp(argument, "off") == 0)
            {
                set_relay_pump(false);
                print_output("Pump off.");
            }
            else
            {
                print_output("Invalid argument. Syntax: 'pump [on/off]'.");
            }
        }
        // RESIZE
        else if (strcmp(command, option_resize) == 0)    
        {
            // Set temp window parameters
            uint width = 9999;
            uint height = 9999;
            uint x_origin = 9999;
            uint y_origin = 9999;

            input = sscanf(argument, "%d %d %d %d", &width, &height, &x_origin, &y_origin);

            // Check if crucial parameters have changed
            if (width == 9999 || height == 9999)
            {
                // If unchanged show user correct syntax
                print_output("Syntax: \"resize [width] [height] [*x origin] [*y origin]\" *optional");
            }
            else
            {
                // If changed draw new window
                clear_ui();
                win_box.height = height;
                win_box.width = width;

                // Check if origin points are changed
                if (x_origin == 9999 || y_origin == 9999)
                {
                    // If unchanged default origin to [1, 1]
                    win_box.x_origin = 1;
                    win_box.y_origin = 1;
                }
                else
                {
                    win_box.x_origin = x_origin;
                    win_box.y_origin = y_origin;
                }
                
                draw_ui();
            }
        }        
        else
        {
            print_output("Invalid command.");
        }
        input_ready = false;
    }
    return 0;
}