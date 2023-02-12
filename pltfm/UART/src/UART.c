#include "UART.h"

 

#define NO_INTERRUPT_REQUEST  (uint32_t)0 
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
  (UART_TX_Node.UART_node->CR1) |= TXEIE_EN;

  //Get user's string info
  //Further jobs would be done in Exception handlers
  
  user_string[UART_TX_Port_u32].container_length = string_length_u32;

  for(int i = 0; i < string_length_u32; i++){
    user_string[UART_TX_Port_u32].container_pointer[i] = byte_string_p[i];
  }

  user_string[UART_TX_Port_u32].container_current_byte = 0;
  user_string[UART_TX_Port_u32].is_last_byte = 0;
  
  //Set TE bit, this would send IDLE frame (first transmission, which caused TXIE interrupt request)
  UART_TX_Node.UART_node->CR1 |= TE_BIT_MASK;

}

/*
  @brief: Causes UART to receive characters from outside
  @appendix: Mechanism of Receiving a character
    During a USART reception, data shifts in LSB first through RX pin. 
    USART_DR consists a buffer RDR between internal bus and the received shift register

    When a character is received:
    1. The RXNE bit is set. It indicates that the content of the shift register is transferred to the
    RDR. In other words, data has been received and can be read (as well as its
    associated error flags).
    2. An interrupt is generated if the RXNEIE bit is set.
    3. The error flags can be set if a frame error, noise or an overrun error has been detected
    during reception.
    4. In multibuffer, RXNE is set after every byte received and is cleared by the DMA read to
    the Data register.
    5. In single buffer mode, clearing the RXNE bit is performed by a software read to the USART_DR register. 
    The RXNE flag can also be cleared by writing a zero to it. The RXNE bit must be cleared before the end of 
    the reception of the next character to avoid an overrun error.

    Note: The RE bit should not be reset while receiving data. If the RE bit is disabled during reception, the 
    reception of the current byte will be aborted
  
  @input:
*/

void UART_Receive_Interrupt(uint32_t UART_TX_Port_u32, uint8_t* byte_string_p){
  //Backbones only, do nothing
  return;
}

void UART_Transmitt_cplt(uint32_t UART_Port_u32){
  //User define
  const UART_cfg UART_TX_Node = UART_conf_cst[UART_Port_u32];
	
  //Disable TE bit
  UART_TX_Node.UART_node->CR1 &= ~TE_BIT_MASK;
	UART_TX_Node.UART_node->CR1 &= ~TCIE_EN;
	UART_TX_Node.UART_node->CR1 &= ~TXEIE_EN;
	
	//clear TC flag 
	UART_TX_Node.UART_node->DR = 0;
  return;
}

void UART_Receive_cplt(void){
  //User define
  return;
}
