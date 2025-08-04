#pragma once

/// @brief Initializes level sensor
void lvl_sens_init();

/// @brief Discharges capacitor
void lvl_sens_discharge();

/// @brief Charges capacitor
void lvl_sens_charge();

/// @brief Reads capacitance
/// @returns Capacitance in uF
float lvl_sens_read_capacitance();
