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


int main()
{
    buckboost_init();

    return 0;
}
