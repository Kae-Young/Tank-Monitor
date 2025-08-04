#pragma once

/// @brief Initializes ultrasonic sensor
void ultrasonic_init();

/// @brief Retrieves distance from ultrasonic sensor
/// @return 
int ultrasonic_distance();

/// @brief Calculates actual volume
/// @return Volume in litres
float ultrasonic_volume();