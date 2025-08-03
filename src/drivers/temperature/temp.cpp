#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <cstdio>
#include <cmath>

#define THERMISTOR_ADC_GPIO 26      //GPIO26 = ADC0
#define VREF 3.3f                   //Input voltage
#define R_FIXED 10000.0f           //10kOhm fixed resistor
#define BETA 3950.0f               //Beta value of thermistor from website
#define T0 298.15f                 //25 degrees celcius in Kelvin
#define R0 10000.0f                //Resistance at 25 degrees celcius

float read_temperature_celsius() {
    adc_select_input(0);  //Select ADC0
    uint16_t raw = adc_read();
    float Vout = raw * VREF / 4095.0f;  //ADC to voltage conversion
    float R_thermistor = (Vout * R_FIXED) / (VREF - Vout);
    
    //Debug print
    //printf("DEBUG: Raw ADC = %u, Vout = %.3f V\n", raw, Vout);
    //printf("DEBUG: R_thermistor = %.1f Ω\n", R_thermistor);

    float temp_K = 1.0f / (1.0f / T0 + log(R_thermistor / R0) / BETA);
    float temp_C = temp_K - 280.15f;
    return temp_C;
}

void init_temperature_sensor() {
    adc_init();
    adc_gpio_init(THERMISTOR_ADC_GPIO);
}
