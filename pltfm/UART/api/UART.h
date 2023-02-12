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
void UART_Transmitt_Interrupt(uint32_t UART_TX_Port_u32, uint8_t* byte_string_p, uint32_t string_length_u32);

void UART_Receive_Interrupt(uint32_t UART_Port_u32);

//Member struct containing user's string
typedef struct{
  uint8_t  container_pointer[100];
  uint32_t container_length;
}container_info;

extern container_info user_string[NUMB_OF_UART_CONFIGURE];

#endif 
