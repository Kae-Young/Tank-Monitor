#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "accelerometer.h"
#include "drivers/board.h"
#include "hardware/i2c.h"
#include "drivers/led/led.h"
#include <math.h>

void accel_init()
{
    i2c_init(I2C_INSTANCE, 400 * 1000);
    gpio_set_function(ACCEL_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(ACCEL_SCL_PIN, GPIO_FUNC_I2C);


    //Verify connection with who am I register

    const uint8_t who_am_i_address = 0x0F;
    const uint8_t expected_who_am_i = 0x33;

    uint8_t data;
    int length = 1;

    if (1 != i2c_write_blocking(I2C_INSTANCE, ACCEL_SAD, &who_am_i_address, length, true))
    {
        log(LogLevel::ERROR, "lis3dh::read_registers: Failed to select WHO_AM_I register address.");
        return;
    }
    
    int bytes_read = i2c_read_blocking(I2C_INSTANCE, ACCEL_SAD, &data, length, false);
    
    if (bytes_read != length) {
        log(LogLevel::ERROR, "lis3dh::read_registers: Failed to read WHO_AM_I data.");
        return;
    }

    if (data != expected_who_am_i)
    {
        log(LogLevel::ERROR, "lis3dh::init_i2c_connection: Failed to verify WHO_AM_I check.");
    }
    
    uint8_t buf[2];

    // Turn normal mode and 1.344kHz data rate on
    const uint8_t CTRL_REG_1 = 0x20;
    buf[0] = CTRL_REG_1;
    buf[1] = 0x97;
    if (1 != i2c_write_blocking(I2C_INSTANCE, ACCEL_SAD, buf, 2, false))
    {
        log(LogLevel::ERROR, "lis3dh::write_registers: Failed to select CTRL_REG_1 register address.");
        return;
    }
}

void lis3dh_calc_value(uint16_t raw_value, float *final_value) 
{
    // Convert with respect to the value being temperature or acceleration reading 
    float scaling;
    float senstivity = 0.004f; // g per unit

    scaling = 64 / senstivity;

    // raw_value is signed
    *final_value = (float) ((int16_t) raw_value) / scaling;
}

void lis3dh_read_data(uint8_t reg, float *final_value) 
{
    // Read two bytes of data and store in a 16 bit data structure
    uint8_t lsb;
    uint8_t msb;
    uint16_t raw_accel;
    i2c_write_blocking(I2C_INSTANCE, ACCEL_SAD, &reg, 1, true);
    i2c_read_blocking(I2C_INSTANCE, ACCEL_SAD, &lsb, 1, false);

    reg |= 0x01;
    i2c_write_blocking(I2C_INSTANCE, ACCEL_SAD, &reg, 1, true);
    i2c_read_blocking(I2C_INSTANCE, ACCEL_SAD, &msb, 1, false);

    raw_accel = (msb << 8) | lsb;

    lis3dh_calc_value(raw_accel, final_value);
}

void spirit_level_update(uint32_t* led_data, uint8_t max_brightness)
{
    //init and clear led
    led_data = led_clear_array(led_data, 12);
    //led_write(led_data);

    // define x and y axes
    float x_accel, y_accel;

    /*  XY polarity map

                              ^
                              |
                             Y=1

                ____________[USB]__________
                |                         |
                |                       1 |
                |                         |
                |                       2 |    
                |                         |     X=1 -->
                |                       3 |
                |                         |
                |                       4 |
                |   8     7     6     5   |
                |_________________________|
    */

    // get current x and y values
    lis3dh_read_data(0x28, &x_accel);
    lis3dh_read_data(0x2A, &y_accel);

    /*  y axis acceleration to LED representation map

        g value |   1   |  0.33  | -0.33  |  -1   |
        -----------------------------------------
        LED     |   4   |   3   |   2   |   1   |

    */

    /*  x axis acceleration to LED representation map

        g value |   1   |  0.33  | -0.33  |  -1   |
        -----------------------------------------
        LED     |   8   |   7   |   6   |   5   |

    */

    // y axis
    if (y_accel <= 0.33)
    {
        if (y_accel <= -0.33)
        {
            // between -0.33 and -1 (incl)

            // calculate LED 2 brightness
            float upper_diff = abs(-0.33 - y_accel);
            float upper_diff_green_brightness_scaler = abs((upper_diff-0.66))/0.66;
            float upper_diff_red_brightness_scaler = upper_diff/0.66;
            int upper_green_brightness = round(max_brightness*upper_diff_green_brightness_scaler);
            int upper_red_brightness = round(max_brightness*upper_diff_red_brightness_scaler);

            // calculate LED 1 brightness
            float lower_diff = abs(-1 - y_accel);
            float lower_diff_green_brightness_scaler = abs((lower_diff-0.66))/0.66;
            float lower_diff_red_brightness_scaler = lower_diff/0.66;
            int lower_green_brightness = round(max_brightness*lower_diff_green_brightness_scaler);
            int lower_red_brightness = round(max_brightness*lower_diff_red_brightness_scaler);

            // set leds
            led_set(2, led_data, led_colour(custom, 0, upper_red_brightness, upper_green_brightness, 0));
            led_set(1, led_data, led_colour(custom, 0, lower_red_brightness, lower_green_brightness, 0));
            led_set(3, led_data, led_colour(red));
            led_set(4, led_data, led_colour(red));
        }
        else
        {
            // between 0.33 and -0.33 (incl)

            // calculate LED 3 brightness
            float upper_diff = abs(0.33 - y_accel);
            float upper_diff_green_brightness_scaler = abs((upper_diff-0.66))/0.66;
            float upper_diff_red_brightness_scaler = upper_diff/0.66;
            int upper_green_brightness = round(max_brightness*upper_diff_green_brightness_scaler);
            int upper_red_brightness = round(max_brightness*upper_diff_red_brightness_scaler);

            // calculate LED 2 brightness
            float lower_diff = abs(-0.33 - y_accel);
            float lower_diff_green_brightness_scaler = abs((lower_diff-0.66))/0.66;
            float lower_diff_red_brightness_scaler = lower_diff/0.66;
            int lower_green_brightness = round(max_brightness*lower_diff_green_brightness_scaler);
            int lower_red_brightness = round(max_brightness*lower_diff_red_brightness_scaler);

            // set leds
            led_set(3, led_data, led_colour(custom, 0, upper_red_brightness, upper_green_brightness, 0));
            led_set(2, led_data, led_colour(custom, 0, lower_red_brightness, lower_green_brightness, 0));
            led_set(1, led_data, led_colour(red));
            led_set(4, led_data, led_colour(red));
        }
    }
    else
    {
        // between 1 (incl) and 0.33 (incl)

        // calculate LED 4 brightness
        float upper_diff = abs(1 - y_accel);
        float upper_diff_green_brightness_scaler = abs((upper_diff-0.66))/0.66;
        float upper_diff_red_brightness_scaler = upper_diff/0.66;
        int upper_green_brightness = round(max_brightness*upper_diff_green_brightness_scaler);
        int upper_red_brightness = round(max_brightness*upper_diff_red_brightness_scaler);

        // calculate LED 3 brightness
        float lower_diff = abs(0.33 - y_accel);
        float lower_diff_green_brightness_scaler = abs((lower_diff-0.66))/0.66;
        float lower_diff_red_brightness_scaler = lower_diff/0.66;
        int lower_green_brightness = round(max_brightness*lower_diff_green_brightness_scaler);
        int lower_red_brightness = round(max_brightness*lower_diff_red_brightness_scaler);

        // set leds
        led_set(4, led_data, led_colour(custom, 0, upper_red_brightness, upper_green_brightness, 0));
        led_set(3, led_data, led_colour(custom, 0, lower_red_brightness, lower_green_brightness, 0));
        led_set(1, led_data, led_colour(red));
        led_set(2, led_data, led_colour(red));
    }

    // x axis
    if (x_accel <= 0.33)
    {
        if (x_accel <= -0.33)
        {
            // between -0.33 and -1 (incl)

            // calculate LED 6 brightness
            float upper_diff = abs(-0.33 - x_accel);
            float upper_diff_green_brightness_scaler = abs((upper_diff-0.66))/0.66;
            float upper_diff_red_brightness_scaler = upper_diff/0.66;
            int upper_green_brightness = round(max_brightness*upper_diff_green_brightness_scaler);
            int upper_red_brightness = round(max_brightness*upper_diff_red_brightness_scaler);

            // calculate LED 5 brightness
            float lower_diff = abs(-1 - x_accel);
            float lower_diff_green_brightness_scaler = abs((lower_diff-0.66))/0.66;
            float lower_diff_red_brightness_scaler = lower_diff/0.66;
            int lower_green_brightness = round(max_brightness*lower_diff_green_brightness_scaler);
            int lower_red_brightness = round(max_brightness*lower_diff_red_brightness_scaler);

            // set leds
            led_set(6, led_data, led_colour(custom, 0, upper_red_brightness, upper_green_brightness, 0));
            led_set(5, led_data, led_colour(custom, 0, lower_red_brightness, lower_green_brightness, 0));
            led_set(7, led_data, led_colour(red));
            led_set(8, led_data, led_colour(red));
        }
        else
        {
            // between 0.33 and -0.33 (incl)

            // calculate LED 7 brightness
            float upper_diff = abs(0.33 - x_accel);
            float upper_diff_green_brightness_scaler = abs((upper_diff-0.66))/0.66;
            float upper_diff_red_brightness_scaler = upper_diff/0.66;
            int upper_green_brightness = round(max_brightness*upper_diff_green_brightness_scaler);
            int upper_red_brightness = round(max_brightness*upper_diff_red_brightness_scaler);

            // calculate LED 6 brightness
            float lower_diff = abs(-0.33 - x_accel);
            float lower_diff_green_brightness_scaler = abs((lower_diff-0.66))/0.66;
            float lower_diff_red_brightness_scaler = lower_diff/0.66;
            int lower_green_brightness = round(max_brightness*lower_diff_green_brightness_scaler);
            int lower_red_brightness = round(max_brightness*lower_diff_red_brightness_scaler);

            // set leds
            led_set(7, led_data, led_colour(custom, 0, upper_red_brightness, upper_green_brightness, 0));
            led_set(6, led_data, led_colour(custom, 0, lower_red_brightness, lower_green_brightness, 0));
            led_set(5, led_data, led_colour(red));
            led_set(8, led_data, led_colour(red));
        }
    }
    else
    {
        // between 1 (incl) and 0.33 (incl)

        // calculate LED 8 brightness
        float upper_diff = abs(1 - x_accel);
        float upper_diff_green_brightness_scaler = abs((upper_diff-0.66))/0.66;
        float upper_diff_red_brightness_scaler = upper_diff/0.66;
        int upper_green_brightness = round(max_brightness*upper_diff_green_brightness_scaler);
        int upper_red_brightness = round(max_brightness*upper_diff_red_brightness_scaler);

        // calculate LED 7 brightness
        float lower_diff = abs(0.33 - x_accel);
        float lower_diff_green_brightness_scaler = abs((lower_diff-0.66))/0.66;
        float lower_diff_red_brightness_scaler = lower_diff/0.66;
        int lower_green_brightness = round(max_brightness*lower_diff_green_brightness_scaler);
        int lower_red_brightness = round(max_brightness*lower_diff_red_brightness_scaler);

        // set leds
        led_set(8, led_data, led_colour(custom, 0, upper_red_brightness, upper_green_brightness, 0));
        led_set(7, led_data, led_colour(custom, 0, lower_red_brightness, lower_green_brightness, 0));
        led_set(5, led_data, led_colour(red));
        led_set(6, led_data, led_colour(red));
    }
    
    led_write(led_data);
}