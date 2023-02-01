#ifndef __UART_CFG_H__
#define __UART_CFG_H__
#include "stm32f10x.h"

/************************
@Author: Huynh Khuong
Define structures to add configurations to UART peripherals
*************************/

/*
  STM32's UART Characteristic:
  - Support 8, 9 bit length
  - Idle character is 1
  - break character is 0 
  - Transmission & Reception are driven by a common baudrate generator 
  - The clock for each side is generated when the enable bit is set respectively for the transmitter and receiver.
*/

/*
  Transmission Characteristic: 
  - Data shifts out LSB first (on the Tx Pin)
  - UART_DR consists of a buffer (TDR) between the internal bus and transmit shift register 
  - Every character is perceived with a start bit, The character is terminated with a number of stop bits 

  - TE bit should not be reset during the transmission of data. 
*/

/*
  Thing we should init for UART
  - Communication side: TX or RX
  - Configurable Stop bits
*/
#endif 
