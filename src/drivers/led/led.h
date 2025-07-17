#pragma once

/// @brief Initialize PIO0 to control the LED chain
/// @returns A pointer to an led data array
uint32_t* led_init();

/// @brief Represents preconfigured colour options
enum colour {
    blue,
    red,
    green,
    white,
    off,
    custom
};

/// @brief Encodes a colour into a structure that the PIO can read
/// @param colour Preconfigured colour options. Put custom for rgb input
/// @param brightness (Optional) LED intensity for preconfigured colours
/// @param r (Optional) Red value
/// @param g (Optional) Green value
/// @param b (Optional) Blue value
/// @returns uint32_t structure that the PIO can read
uint32_t led_colour(colour colour = custom, int brightness = 255, int r = 0, int g = 0, int b = 0);

/// @brief Updates the LED data by changing the value of one LED
/// @param led_num Address of LED
/// @param led_data A handle on the current LED data array
/// @param value (Optional) Ouput from led_colour
/// @return The newly update LED data aray
uint32_t* led_set(int led_numm, uint32_t* led_data, uint32_t value = 0);

/// @brief Writes current LED data to LEDs
/// @param led_data A handle on the current LED data array
void led_write(uint32_t* led_data);

/// @brief Writes off to all LEDs
void led_off();

/// @brief Blinks the first LED once
void led_blink();

/// @brief Plays a preconfigured light demo
void led_demo();

/// @brief Returns array of zeros
/// @param led_data Array to clear
/// @param arr_size Size of array
/// @return Array of zeros
uint32_t* led_clear_array(uint32_t* led_data, int arr_size);

/// @brief Short preconfigured device start up LED show
void led_device_startup_show();