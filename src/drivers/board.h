#pragma once

//pin definitions
#define LED_PIN 28

#define BUCKBOOST_SCL_PIN 2
#define BUCKBOOST_SDA_PIN 3

#define LVL_SENS_CHARGE_PIN 18
#define LVL_SENS_DISCHARGE_PIN 20
#define LVL_SENS_READ_PIN 28

#define TRIG_PIN 22
#define ECHO_PIN 20

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define relay_solenoid 13
#define relay_motor 25
#define SW1 15

//buck and boost converter communication
#define BUCKBOOST_SLAVE_ADDRESS 0x75
#define I2C_INSTANCE i2c1
#define VOUT_REGISTER 0x04 

//bucket dimensions
#define BUCKET_HEIGHT_CM 20.5
#define BUCKET_RADIUS 12