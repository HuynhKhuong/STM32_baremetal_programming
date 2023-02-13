#ifndef __UART_EX_H__
#define __UART_EX_H__

#include "UART.h"

//APIs for Exception task
void UART_Transmit_Exception_call(uint32_t UART_Port_index);
void UART_Receive_Exception_call(uint32_t UART_Port_index);

#endif
