#pragma once

/// @brief Initialize I2C communication with accelerometer
void accel_init();

void lis3dh_calc_value(uint16_t raw_value, float *final_value);

void lis3dh_read_data(uint8_t reg, float *final_value);

/// @brief Updates LEDs based on a graphical representation of accelerometer data. Must call accel_init() before using.
/// @param led_data A handle on the current LED data array
/// @param max_brightness Maximum LED brightness value between 0 and 255
void spirit_level_update(uint32_t* led_data, uint8_t max_brightness);