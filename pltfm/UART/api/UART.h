#ifndef __UART_H__
#define __UART_H__

#include "UART_cfg.h"

//APIs for Init task
void UART_Init(void);

/*APIs for Normal task:
  - Transmit
  - Receive
  These tasks would corporate the Exception logic to to the process
*/
void UART_Transmitt_Interrupt(uint32_t UART_Port_u32);
void UART_Receive_Interrupt(uint32_t UART_Port_u32);

#endif 
