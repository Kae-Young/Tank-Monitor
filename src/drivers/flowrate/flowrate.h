#pragma once

#include <stdint.h>
#include "hardware/gpio.h"

void pulse_handler(uint gpio, uint32_t events);
void flow_sensor_init();
void flow_data();

extern volatile double waterFlow;