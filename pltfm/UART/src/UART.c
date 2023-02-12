#include "UART.h"

#define TE_BIT_POS    (uint32_t)3
#define TE_BIT_MASK   USART_CR1_TE          

//Internal container declaration
container_info user_string[NUMB_OF_UART_CONFIGURE];
/*
  @brief: Causes a UART Port to transmit a string of bytes, using interrupt mechanism
  @input: Index of UART Node in main configuration struct, pointer to string of bytes
  @output: UART would transmit string of bytes 

  @appendix: Mechanism of transmitting of UART

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

void UART_Transmitt_Interrupt(uint32_t UART_TX_Port_u32, uint8_t* byte_string_p, uint32_t string_length_u32){
/*
  Procedure of Transmitting a character
  1. Set TE to send an IDLE frame and first transmission 
  2. Write Data to send in the DR register (this clears the TXE bit)
  3. After writing the last data into the UART_DR register, wait until TC= 1. This indicates that 
  the transmission of the last frame is complete
  4. Repeat the this for each data in case of single buffer 
*/
  const UART_cfg UART_TX_Node = UART_conf_cst[UART_TX_Port_u32];
  uint32_t is_transmission_occuring = 0; //default value is 0 

  //First check whether the current index is TX or not
  if(UART_TX_Node.Comm_DIR == RX) return;  

  //Second check whether there are transmission occuring
  is_transmission_occuring = ((UART_TX_Node.UART_node->CR1) & TE_BIT_MASK) >> TE_BIT_POS;
  if(is_transmission_occuring) return;

  //Enable all necessary Interrupt requests
  (UART_TX_Node.UART_node->CR1) |= UART_TX_Node.Interrupt_enquire.request1_u32; //Complete a single byte transmission only

  //Store string information for further processing

  //Set TE bit, this would send IDLE frame (first transmission, which caused TXIE interrupt request)
  UART_TX_Node.UART_node->CR1 |= TE_BIT_MASK;

  //Get user's string info
  //Further jobs would be done in Exception handlers
  
  user_string[UART_TX_Port_u32].container_length = string_length_u32;

  for(int i = 0; i < string_length_u32; i++){
    user_string[UART_TX_Port_u32].container_pointer[i] = byte_string_p[i];
  }
}

void UART_Receive_Interrupt(uint32_t UART_Port_u32){
  //Backbones only, do nothing
  return;
}
