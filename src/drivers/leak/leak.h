#pragma once
//If flow = 0 while the pump and valve relays are turned on then this function runs and prints the appropriate error message.
void check_for_blockage();
//This is a combined leak detection function that check for leaks when there is no flow 
//and when the actual water level does not line up with where it should be for the water output detected by the flow rate sensor
void check_for_leak(double current_flow_rate, double current_measured_volume, double current_actual_volume);