#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "drivers/board.h"
#include "hardware/i2c.h"
#include "drivers/led/led.h"
#include <math.h>

void buckboost_init()
{
    i2c_init(I2C_INSTANCE, 400 * 1000);
    gpio_set_function(BUCKBOOST_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(BUCKBOOST_SCL_PIN, GPIO_FUNC_I2C);

    // who am I register doesnt exist

    uint8_t data;
    int length = 1;
    
    uint8_t buf[2];

    // Set buckboost vout to 5v
    const uint8_t VOUT_REG = 0x04;
    buf[0] = 0x04;
    buf[1] = 0b01110010; //was getting vout of 5.14v on the 5v register of the buckboost conveter, changed to the 4.875v register to get an outtput voltage of 5V
    i2c_write_blocking(I2C_INSTANCE, BUCKBOOST_SLAVE_ADDRESS, buf, 2, false);

    sleep_ms(1000);
}