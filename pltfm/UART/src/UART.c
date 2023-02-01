#include "UART.h"

/*
  Setting TE bit on causes the MCU to send the IDLE frame very first
  When transmitting multiple characters: 
  - TXE is always cleared by a write to the data register
  - TXE is set by hardware which indicates: 
    + data is moved from TDR -> Shift register (data transmission has been started)
    + TDR is empty 
    + next data can be written 
  When a transmission is taking place the DR register stores the data into the TDR register
  when there are no tranmissions going, a write into DR register goes directly into Shift register

  After a frame is transmitted (completely out of Shift register): 
  - TXE = 1, 
  - TC = 1,
  
  After writing the last data into the UART_DR register, wait TC = 1 before disabling the UART.  

  TC bit is cleared by: 
  1. A read from the UART_SR register
  2. A write to UART_DR register 
*/
