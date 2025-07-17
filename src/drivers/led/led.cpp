#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"
#include "led.h"
#include "drivers/board.h"

uint32_t* led_init()
{
    // Initialise PIO0 to control the LED chain
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);

    // Initialise led data array
    static uint32_t led_data[12];
    for (int i = 0; i < 12; i++)
    {
        led_data[i] = 0;
    }
    
    return led_data;
}

uint32_t led_colour(colour colour, int brightness, int r, int g, int b)
{
    switch (colour)
    {
    case red:
        r = 1 * brightness;
        g = 0 * brightness;
        b = 0 * brightness;
        break;
    
    case green:
        r = 0 * brightness;
        g = 1 * brightness;
        b = 0 * brightness;
        break;
    
    case blue:
        r = 0 * brightness;
        g = 0 * brightness;
        b = 1 * brightness;
        break;
    
    case white:
        r = 1 * brightness;
        g = 1 * brightness;
        b = 1 * brightness;
        break;
    
    case off:
        r = 0 * brightness;
        g = 0 * brightness;
        b = 0 * brightness;
        break;
    
    default:
        break;
    }
    return (r << 24) | (g << 16) | (b << 8);
}

uint32_t* led_set(int led_num, uint32_t* led_data, uint32_t value)
{
    led_data[led_num-1] = value;
    return led_data;
}

void led_write(uint32_t* led_data) 
{
    for (int i = 0; i < 12; i++)
    {
        pio_sm_put_blocking(pio0, 0, led_data[i]);
    }
}

void led_off()
{
    uint32_t j [12] = { };
    for (int i = 0; i < 12; i++)
    {
        pio_sm_put_blocking(pio0, 0, j[i]);
    }
}

void led_blink()
{
    uint32_t* led_data = led_init();
    led_off();
    sleep_ms(400);
    led_data = led_set(1, led_data, led_colour(white, 150));
    led_write(led_data);
    sleep_ms(400);
    led_off();
}

uint32_t* led_clear_array(uint32_t* led_data, int arr_size)
{
    for (int i = 0; i < arr_size+1; i++)
    {
        led_data[i] = 0;
    }
    return led_data;
}

void led_device_startup_show()
{
    uint32_t* led_data = led_init();
    
    for (int i = 1; i < 13; i++)
    {
        led_data = led_clear_array(led_data, 12);
        led_data = led_set(i, led_data, led_colour(white, 5));
        led_write(led_data);
        sleep_ms(50);
    }
    for (int i = 12; i > 0; i--)
    {
        led_data = led_clear_array(led_data, 12);
        led_data = led_set(i, led_data, led_colour(white, 5));
        led_write(led_data);
        sleep_ms(50);
    }
}

void led_demo()
{
    uint32_t* led_data = led_init();

    for (int i = 1; i < 13; i++)
    {
        led_data = led_clear_array(led_data, 12);
        led_data = led_set(i, led_data, led_colour(white, 5));
        led_write(led_data);
        sleep_ms(50);
    }
    for (int i = 12; i > 0; i--)
    {
        led_data = led_clear_array(led_data, 12);
        led_data = led_set(i, led_data, led_colour(white, 5));
        led_write(led_data);
        sleep_ms(50);
    }
    for (int i = 1; i < 13; i++)
    {
        led_data = led_set(i, led_data, led_colour(white, 5*i));
        led_write(led_data);
        sleep_ms(50);
    }
    for (int i = 12; i > 0; i--)
    {
        led_data = led_set(i, led_data, led_colour(white));
        led_write(led_data);
        sleep_ms(50);
    }
    for (int i = 0; i < 200; i += 40)
    {
        for (int j = 0; j < 13; j++)
        {
            led_data = led_set(j, led_data, led_colour(custom,0, 255 - i, 255, 255));
        }
        led_write(led_data);
        sleep_ms(60);
    }
    for (int i = 0; i < 200; i += 40)
    {
        for (int j = 0; j < 13; j++)
        {
            led_data = led_set(j, led_data, led_colour(custom,0, 55 + i, 255 - i, 255));
        }
        led_write(led_data);
        sleep_ms(60);
    }
    for (int i = 0; i < 200; i += 40)
    {
        for (int j = 0; j < 13; j++)
        {
            led_data = led_set(j, led_data, led_colour(custom,0, 255, 55 + i, 255 - i));
        }
        led_write(led_data);
        sleep_ms(60);
    }
    for (int i = 0; i < 281; i += 40)
    {
        for (int j = 0; j < 13; j++)
        {
            led_data = led_set(j, led_data, led_colour(custom,0, 255 - i, 255 - i, 55 + i));
        }
        led_write(led_data);
        sleep_ms(60);
    }
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 13; j += 2)
        {
            led_data = led_set(j, led_data, led_colour(blue, 150));
        }
        for (int j = 1; j < 13; j += 2)
        {
            led_data = led_set(j, led_data, led_colour(off));
        }
        led_write(led_data);
        sleep_ms(100);
        for (int j = 1; j < 13; j += 2)
        {
            led_data = led_set(j, led_data, led_colour(red, 150));
        }
        for (int j = 0; j < 13; j += 2)
        {
            led_data = led_set(j, led_data, led_colour(off));
        }
        led_write(led_data);
        sleep_ms(100);
    }
    led_off();
}