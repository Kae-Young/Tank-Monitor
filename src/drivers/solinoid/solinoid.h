#pragma once

void solinoid_setup();

void toggle_relay_solenoid(); 
//Checks solenoid status to use in leak/blockage testing cpp
bool is_solenoid_open();

void set_relay_solenoid(bool state);

void set_relay_pump(bool state);
