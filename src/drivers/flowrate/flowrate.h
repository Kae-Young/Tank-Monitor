#pragma once

#include <stdint.h>
#include "hardware/gpio.h"

/// @brief Increments water_volume and pulse_count upon rising edge interupt
void pulse_handler(uint gpio, uint32_t events);

/// @brief Initializes flow rate sensor
void water_flow_init();

void flow_rate_calc();

void flow_rate_average();

double flow_rate();

double measured_volume();