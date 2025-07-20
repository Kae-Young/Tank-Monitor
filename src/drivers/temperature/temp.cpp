#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <math.h>
#include <cstdio>

// Constants based on your thermistor and voltage divider setup
#define R_FIXED 10000.0f       // 10kΩ resistor in series with thermistor
#define BETA 3950.0f           // Beta constant for thermistor
#define T0 298.15f             // Nominal temperature (25°C in Kelvin)
#define R0 10000.0f            // Thermistor resistance at 25°C
#define VREF 3.3f              // ADC reference voltage
#define ADC_MAX 4095.0f        // 12-bit ADC resolution
uint16_t raw = adc_read();  // only inside temperature.cpp

void init_temperature_sensor() {
    adc_init();             // Initialise ADC peripheral
    adc_gpio_init(26);      // Enable ADC function on GPIO26
    adc_select_input(0);    // Select ADC input channel 0
}

float read_temperature_celsius() {
    uint16_t raw = adc_read();
    float Vout = (raw / ADC_MAX) * VREF;

    // Prevent divide-by-zero or domain error
    if (Vout >= VREF) Vout = VREF - 0.001f;

    // Calculate thermistor resistance using voltage divider formula
    float R_thermistor = (VREF * R_FIXED / Vout) - R_FIXED;

    // Protect against log(0) or negative values
    if (R_thermistor <= 0.0f) return -100.0f;

    // Apply Beta equation to calculate temperature
    float temp_K = 1.0f / ((1.0f / BETA) * log(R_thermistor / R0) + (1.0f / T0));
    float temp_C = temp_K - 273.15f;

    // Optional debug print
    printf("Raw ADC: %u | Vout: %.3f V | R_thermistor: %.1f Ω | Temp: %.2f °C\n",
           raw, Vout, R_thermistor, temp_C);

    return temp_C;
}
