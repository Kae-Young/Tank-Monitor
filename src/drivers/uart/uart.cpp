#include "drivers/uart/terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <drivers/board.h>

// uart stuff
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))   //LEN(arr) for number of rows //LEN(arr[0]) for number of columns

// uart stuff
static int chars_rxed = 0;
volatile char buffer [100];
volatile unsigned int myIndex = 0; 
volatile bool input_ready = false;

/* 
################################################################
                        UI FRAMEWORK
################################################################                                             
*/
// UI box struct contains height and width information
typedef struct box {
    int width;
    int height;
    int x_origin;
    int y_origin;
    char *header;
    bool is_heading_centered;
}   box_T;

// declare boxes
box_T win_box;
box_T readings_box;
box_T opt_box;
box_T in_box;
box_T out_box;

void clear_ui() {
    term_move_to(win_box.x_origin,win_box.y_origin);
    term_set_color(clrBlack, clrBlack);
    for (int i = 0; i < win_box.height + 1; i++)
    {
        for (int j = 0; j < win_box.width + 1; j++)
        {
            printf(" ");
            if(j == win_box.width)   {
                printf(" \r\n");
            }
        } 
    }  
}

// Draw heading
void draw_heading(box_T b) {
    //set colour
    term_set_color(clrBlack, clrGreen);

    int x_cursor_location; //set horizontal cursor
    if(b.is_heading_centered)
    {
        x_cursor_location = round(((b.width+2)-strlen(b.header))/2) + b.x_origin; //set to center of window
    }
    else
    {
        x_cursor_location = b.x_origin + 2; //set left aligned
    }
    int y_cursor_location = b.y_origin + 1; //set cursor to 1 line below top of box
    
    term_move_to(x_cursor_location, y_cursor_location);
    printf(b.header);
}

void draw_box(box_T b)    {
    //set colour
    term_set_color(clrGreen, clrBlack);

    //draw top border
    term_move_to(b.x_origin, b.y_origin);
    printf("+");
    for (int i = 0; i < b.width - 2; i++)
    {
        printf("-");
    }
    printf("+");

    //draw bottom border
    term_move_to(b.x_origin, b.y_origin + b.height);
    printf("+");
    for (int i = 0; i < b.width - 2; i++)
    {
        printf("-");
    }
    printf("+");

    //draw left border
    for (int i = 1; i < b.height; i++)
    {
        term_move_to(b.x_origin, b.y_origin + i);
        printf("|");
    }

    //draw right border
    for (int i = 1; i < b.height; i++)
    {
        term_move_to(b.x_origin + b.width-1, b.y_origin + i);
        printf("|");
    }

    if (b.header != "\0")
    {
        draw_heading(b);
    }
}

// Clear input box
void clr_input()     {
    term_set_color(clrGreen, clrBlack);
    int x_cursor = in_box.x_origin + 5;
    int y_cursor = in_box.y_origin + 2;
    term_move_to(x_cursor, y_cursor);
    printf("                                                                    ");
    term_move_to(x_cursor, y_cursor);
}
// Clear output box
void clr_output()    {
    term_set_color(clrGreen, clrBlack);
    int x_cursor = out_box.x_origin + 2;
    int y_cursor = out_box.y_origin + 2;
    term_move_to(x_cursor, y_cursor);
    printf("                                                                    ");
}

// Print to output box
void print_output(char output[])  {
    clr_output();
    term_set_color(clrGreen, clrBlack);
    int x_cursor = out_box.x_origin + 2;
    int y_cursor = out_box.y_origin + 2;
    term_move_to(x_cursor, y_cursor);
    uart_puts(UART_ID, output);
    clr_input();
}

const int readings_text_width = 25;
const int readings_text_height = 5;
// Print coordinates
void print_readings(double readings[]) {
    term_set_color(clrGreen, clrBlack);
    //set cursor position
    int x_cursor = round(((opt_box.width+2) - readings_text_width)/2 + opt_box.x_origin) + 18;
    int y_cursor = round(((opt_box.height+2) - readings_text_height)/2 + opt_box.y_origin);
    for (int i = 0; i < readings_text_height; i++)
    {
        term_move_to(x_cursor, i + y_cursor);
        if (i < 3)
        {
            printf("%.2f L     ", readings[i]);
        }
        else if (i == 3)
        {
            printf("%.2f C     ", readings[i]);
        }
        else
        {
            printf("%.0f uS    ", readings[i]);
        }
    }
    term_set_color(clrGreen, clrBlack);
    x_cursor = in_box.x_origin + 5;
    y_cursor = in_box.y_origin + 2;
    term_move_to(x_cursor, y_cursor);
}

void fill_opt_box()
{
    // Draw options box contents
    term_set_color(clrGreen, clrBlack);
    char options[6][47] = {"menu - return to menu", "read - see readings", "reset - reset expected volume to actual volume", "valve - open/close valve", "pump - turn on/off pump"};
    int num_of_options = LEN(options);
    int max_length = LEN(options[2]);
    int x_cursor = round(((opt_box.width+2) - max_length)/2 + opt_box.x_origin);
    int y_cursor = round(((opt_box.height+2) - num_of_options)/2 + opt_box.y_origin);
    for (int i = 0; i < num_of_options; i++)
    {
      term_move_to(x_cursor, y_cursor + i);
      printf(options[i]);
    }
}

void fill_readings_box()
{
    // Draw readings box contents
    term_set_color(clrGreen, clrBlack);
    int x_cursor = round(((readings_box.width+2) - readings_text_width)/2 + readings_box.x_origin);
    int y_cursor = round(((readings_box.height+2) - readings_text_height)/2 + readings_box.y_origin);
    char readings[5][16] = {"Expected Volume", "Actual Volume", "Leakage Volume", "Temperature", "Elapsed Time"};
    for (int i = 0; i < readings_text_height; i++)
    {
        term_move_to(x_cursor, i + y_cursor);
        printf(readings[i]);
        term_move_to(x_cursor + 16, i + y_cursor);
        printf(":");
    }
}

// Draw UI
void draw_ui(bool readings_mode = false)  {
    //the window is made up of a 9x9 grid
    double width = win_box.width;
    double height = win_box.height;
    double x_grid_step = width/9;
    double y_grid_step = height/9;

    // Configure readings box
    readings_box.width = round(7*x_grid_step);
    readings_box.height = round(3*y_grid_step);
    readings_box.x_origin = win_box.x_origin + round(1*x_grid_step);
    readings_box.y_origin = win_box.y_origin + round(1*y_grid_step);
    readings_box.header = "Readings";
    readings_box.is_heading_centered = true;

    // Configure options box
    opt_box.width = round(7*x_grid_step);
    opt_box.height = round(3*y_grid_step);
    opt_box.x_origin = win_box.x_origin + round(1*x_grid_step);
    opt_box.y_origin = win_box.y_origin + round(1*y_grid_step);
    opt_box.header = "Options";
    opt_box.is_heading_centered = true;

    // Configure input box
    in_box.width = round(opt_box.width);
    in_box.height = round(1.5*y_grid_step);                                  
    in_box.x_origin = opt_box.x_origin;                                     //aligned horizontally with xyz_box
    in_box.y_origin = win_box.y_origin + round(opt_box.height + 2*y_grid_step);                    
    in_box.header = "Input";
    in_box.is_heading_centered = false;

    //Configure output box
    out_box.width = in_box.width;                       //equal width as input box
    out_box.height = in_box.height;                     //equal height as output box
    out_box.x_origin = in_box.x_origin;                 //horizontally aligned with input box
    out_box.y_origin = in_box.y_origin + in_box.height; //just below input box
    out_box.header = "Output";
    out_box.is_heading_centered = false;

    // Draw UI frame
    clear_ui();
    draw_box(win_box);
    if (readings_mode)
    {
        draw_box(readings_box);
        fill_readings_box();
    }
    else
    {
        draw_box(opt_box);
        fill_opt_box();
    }
    draw_box(in_box);
    draw_box(out_box);

    // Draw input ready
    int x_cursor = in_box.x_origin + 3;
    int y_cursor = in_box.y_origin + 2;
    term_move_to(x_cursor, y_cursor);
    term_set_color(clrGreen, clrBlack);
    printf("> ");

    print_output("Ready for commands...\n");
    clr_input();
}

/*
###############################################################
                    END OF UI FRAMEWORK
###############################################################
*/

// Write character
void send_ch(char ch)   {
    if(uart_is_writable(UART_ID))   {
        uart_putc(UART_ID, ch);
    }
}

// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
        // Echo back the character received
        send_ch(ch);
        // Detect character
        switch (ch) {
            // Check if 'Enter' is pressed
            case '\n':                          
                buffer[myIndex] = 0;
                myIndex = 0;
                input_ready = true;
                break;
            case '\r':
                buffer[myIndex] = 0;
                clr_input();
                myIndex = 0;
                input_ready = true;
                break;
            // Backspace handling
            case '\177':                        
                if (myIndex > 0) {
                    myIndex--;
                    buffer[myIndex] = '\000';
                }
                break;
            // Save the character to buffer
            default:
                if (myIndex <= 98)  {           
                    buffer[myIndex] = ch;      
                    myIndex++; 
                }
        }
        chars_rxed++;
    }
}

void ui_init()
{
     // Set up UART
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);

    // Configure window box
    // TIP: UI works better when width and height are multiples of 9
    win_box.width = 150;                        //set box width
    win_box.height = 33;                        //set box height
    win_box.x_origin = 5;                       //set box x origin
    win_box.y_origin = 5;                       //set box y origin
    win_box.header = "Tank Monitor";     //set box header
    win_box.is_heading_centered = true;         //set heading alignment

    draw_ui();
}