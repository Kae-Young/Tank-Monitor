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

//update
int main()
{
    led_blink();
    buckboost_init();
    init_temperature_sensor();

    uint32_t* led_data = led_init();
    led_set(1, led_data, led_colour(red));
    led_write(led_data);

    stdio_init_all();
    level_sensor_init();
    
    while (true) {
        float temp = read_temperature_celsius();
        printf("Temperature: %.2f °C\n", temp);
        sleep_ms(2000);

        printf("%.6f \r\n", level_sensor_read_capacitance());
        sleep_ms(3000);
    }
    return 0;
}
