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

/*void check_for_static_leak()
{
    const double level_drop_threshold = 0.5;
    //Leak detected water level is dropping but no flow through flow rate sensor
    if (flowRate == 0 && (measuredVolume - ultrasonicVolume) > level_drop_threshold)
    {
        //To add flash_blue, flash_red and flash_green
        //flash_red();
        printf("Water level dropping with no flow. Check for leaks.\n");
    }
}

void check_for_flow_mismatch()
{
    const double level_drop_threshold = 1.0;
    //Leak detected more water lost than flow sensor indicates

    if (flowRate > 0 && (measuredVolume - ultrasonicVolume) > level_drop_threshold)
    {
        //To add flash_blue, flash_red and flash_green functions
        //flash_blue();
        printf("More water lost than recorded volume output.\n");
    }
}*/

//Combined Leak Detection
void check_for_leak(double current_flow_rate, double current_measured_volume, double current_actual_volume)
{
    const double level_drop_threshold = 0.5;

    //Leak detected water level is dropping but no flow through flow rate sensor
    if (current_flow_rate == 0 && (current_measured_volume - current_actual_volume) > level_drop_threshold)
    {
        //To add flash_blue, flash_red and flash_green
        //flash_red();
        printf("Water level dropping with no flow. Check for leaks.\n");
    }

    //Leak detected more water lost than flow sensor indicates
    if (current_flow_rate > 0 && (current_measured_volume - current_actual_volume) > level_drop_threshold)
    {
        //To add flash_blue, flash_red and flash_green functions
        //flash_blue();
        printf("More water lost than recorded volume output.\n");
    }
}

void check_for_blockage()
{
    //const uint32_t delay_ms = 2000;  // Wait 2 seconds after opening
    const double flow_threshold = 0.01; // Minimum flow rate expected when open

    if (is_solenoid_open()) {
        //sleep_ms(1000);  // Give time for flow to start

        double flowRate = flow_rate();
        if (flowRate < flow_threshold) {
            //flash_green();  // You can choose any LED pattern
            printf("Solenoid is open but no water is flowing. Possible blockage detected.\n");
        }
    }
}
