#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "led.h"
#include "drivers/board.h"
#include "drivers/flowrate/flowrate.h"
#include "drivers/ultrasonic/HC-SR04.h"


void check_for_static_leak()
{
    const double level_drop_threshold = 1.0;
    //Leak detected water level is dropping but no flow through flow rate sensor
    double flowRate = flow_rate();
    double measuredVolume = measured_volume();
    double ultrasonicVolume = ultrasonic_volume();

    if (flowRate == 0 && (measuredVolume - ultrasonicVolume) > level_drop_threshold)
    {
        //To add flash_blue, flash_red and flash_green
        flash_red();
        printf("Water level dropping with no flow. Check for leaks.");
    }
}

void check_for_flow_mismatch()
{
    const double level_drop_threshold = 1.0;
    //Leak detected water level is dropping but no flow through flow rate sensor
    double flowRate = flow_rate();
    double measuredVolume = measured_volume();
    double ultrasonicVolume = ultrasonic_volume();

    if (flowRate > 0 && (measuredVolume - ultrasonicVolume) > level_drop_threshold)
    {
        //To add flash_blue, flash_red and flash_green functions
        flash_blue();
        printf("More water lost than recorded volume output.");
    }
}

