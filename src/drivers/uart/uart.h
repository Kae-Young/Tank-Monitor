#pragma once

extern bool input_ready;

extern char buffer[100];

void print_readings(double readings[]);

void clear_ui();

void clr_input();

void clr_output();

void print_output(char output[]);

void draw_ui(bool readings_mode = false);

void ui_init();