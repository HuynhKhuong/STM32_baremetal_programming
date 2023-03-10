#ifndef __UART_H__
#define __UART_H__

#include "UART_cfg.h"

#define TXEIE_EN      USART_CR1_TXEIE 
#define TCIE_EN       USART_CR1_TCIE
#define TE_BIT_POS    (uint32_t)3
#define TE_BIT_MASK   USART_CR1_TE         

#define RE_BIT_POS    (uint32_t)2
#define RE_BIT_MASK   USART_CR1_RE

#define RXNEIE_EN     USART_CR1_RXNEIE
#define IDLEIE_EN     USART_CR1_IDLEIE

#define User_define_RX	//User defines this if they want to add their own code once the Reception is done

//APIs for Init task
void UART_Init(void);

/*APIs for Normal task:
  - Transmit
  - Receive
  These tasks would corporate the Exception logic to to the process
*/
void UART_Transmitt_Interrupt(uint32_t UART_TX_Port_u32, uint8_t* byte_string_p, uint32_t string_length_u32);
void UART_Receive_Interrupt(uint32_t UART_TX_Port_u32, uint8_t* byte_string_p);


/*
  APIs for user to handle when communication is done 
*/
//Handle the final character
void UART_Transmitt_cplt(uint32_t UART_Port_u32);
void UART_Receive_cplt(uint32_t UART_Port_u32);

//These functions would be invoked when UART_Tx/Rx_cplt is called
//These functions are User define completely, would be defined in main

#ifdef User_define_TX
void User_GP_UART_Transmit_cplt(void);
#endif 

#ifdef User_define_RX
void User_GP_UART_Receive_cplt(void);
#endif

//Member struct containing user's string
typedef struct{
  uint8_t  container_pointer[100];
  uint32_t container_length;
  uint32_t container_current_byte;
  uint32_t is_last_byte;
  uint8_t* user_provided_string; //This pointer is for RX only
}container_info;

extern container_info user_string[NUMB_OF_UART_CONFIGURE*2]; //*2 as we need to seperate Rx and Tx buffer

#endif 
