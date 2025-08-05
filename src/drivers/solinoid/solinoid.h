#pragma once

void solinoid_setup();

void toggle_relay_solenoid(); 
//Checks solenoid status to use in leak/blockage testing cpp
bool is_solenoid_open();
