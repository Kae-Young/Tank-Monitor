#pragma once

#include <stdint.h>
#include "hardware/gpio.h"

void pulse_handler(uint gpio, uint32_t events);
void waterFlowData();
void flow_rate_calc();
void flow_rate_average();
void flow_data();

extern volatile double waterFlow;
extern volatile double flow_rate;