#pragma once

void check_for_static_leak();

void check_for_flow_mismatch();

void check_for_blockage();

void check_for_leak(double current_flow_rate, double current_measured_volume, double current_actual_volume);