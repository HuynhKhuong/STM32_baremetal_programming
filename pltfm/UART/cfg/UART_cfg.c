#include "UART.h"

/*
  Procedure of Transmitting a character
  1. Enable UART: UE bit
  2. M bit to define the word length
  3. Program the number of stop bit 
  4. DMA enable (optional)
  5. Select the desired baudrate
  /-------------- Configuring phase --------------------/
  6. Set TE to send an IDLE frame and first transmission 
  7. Write Data to send in the DR register (this clears the TXE bit)
  8. After writing the last data into the UART_DR register, wait until TC= 1. This indicates that 
  the transmission of the last frame is complete
  9. Repeat the this for each data in case of single buffer 
*/