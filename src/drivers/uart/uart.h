#pragma once

// UI box struct contains height and width information
typedef struct box {
    int width;
    int height;
    int x_origin;
    int y_origin;
    char *header;
    bool is_heading_centered;
}   box_T;

extern box_T win_box;

extern bool input_ready;

extern char buffer[100];


void print_readings(double readings[]);

void clear_ui();

void clr_input();

void clr_output();

void print_output(char output[]);

void draw_ui(bool readings_mode = false);

void ui_init();