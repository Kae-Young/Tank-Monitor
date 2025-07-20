#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <math.h>
#include <cstdio>

/* #define R_FIXED 10000.0f       // 10k ohm fixed resistor
#define BETA 3950.0f           // Thermistor beta value
#define T0 298.15f             // Reference temperature (25°C in Kelvin)
#define R0 9200.0f            // Thermistor resistance at T0
#define VREF 3.3f              // ADC reference voltage
#define ADC_MAX 4095.0f        // 12-bit ADC maximum

void init_temperature_sensor() {
    adc_init();             // Initialise the ADC peripheral
    adc_gpio_init(26);      // Enable ADC function on GPIO26
    adc_select_input(0);    // Select ADC0 (GPIO26)
}

float read_temperature_celsius() {
    uint16_t raw = adc_read();  // Raw ADC reading (0–4095)
    float Vout = (raw / ADC_MAX) * VREF;

    // Calculate thermistor resistance from voltage divider
    float R_thermistor = (VREF * R_FIXED / Vout) - R_FIXED;

    // Apply Beta equation
    float temp_K = 1.0f / ((1.0f / BETA) * log(R_thermistor / R0) + (1.0f / T0));
    return temp_K - 273.15f;  // Convert to Celsius
}
*/
