#include "I2C.h"

// @author: Huynh Khuong
/*
  Init procedure for master: 
  1. Program the I2C_CR2 register -> program the peripheral input clock 
  2. Configure the clock control register
  3. Configure the rise time register
  4. Configure the I2C_CR2 register -> Enable I2C
  5. Set the START_BIT To generate start condition

  Note that: the peripheral input clock must be at least: 
  - 2 MHz in standard speed
  - 4 MHz in fast speed
*/
