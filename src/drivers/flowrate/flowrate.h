#pragma once

#include <stdint.h>
#include "hardware/gpio.h"

void pulse_handler(uint gpio, uint32_t events);
void waterFlowData();

extern volatile double waterFlow;