#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/led/led.h"
#include "drivers/board.h"
#include "drivers/flowrate/flowrate.h"
#include "drivers/ultrasonic/HC-SR04.h"
#include "drivers/solinoid/solinoid.h"
#include "drivers/uart/uart.h"

//Combined Leak Detection
void check_for_leak(double current_flow_rate, double current_measured_volume, double current_actual_volume)
{
    const double level_drop_threshold = 0.5;
    //Leak detected water level is dropping but no flow through flow rate sensor
    if (current_flow_rate == 0 && (current_measured_volume - current_actual_volume) > level_drop_threshold)
    {
        //Would add flash_blue, flash_red and flash_green functions if LEDs worked.
        //flash_red();
        print_output("Water level dropping with no flow. Check for leaks.\n");
    }
    //Leak detected more water lost than flow sensor indicates
    if (current_flow_rate > 0 && (current_measured_volume - current_actual_volume) > level_drop_threshold)
    {
        //Would add flash_blue, flash_red and flash_green functions if LEDs worked.
        //flash_blue();
        print_output("More water lost than recorded volume output.\n");
    }
}

//This function is only called upon when the solenoid valve is open. Checks the detected flow rate to see if it passes the threshold set.
//Then prints blockage detection message to UART.
void check_for_blockage()
{
    const double flow_threshold = 0.01; //Minimum flow rate expected when open
    if (is_solenoid_open()) {
        double flowRate = flow_rate();
        if (flowRate < flow_threshold) {
            //Would add flash_blue, flash_red and flash_green functions if LEDs worked.
            //flash_green();
            print_output("Solenoid is open but no water is flowing. Possible blockage detected.\n");
        }
    }
}
