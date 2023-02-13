#include "UART_ex.h"

#define TXE_BIT_MASK USART_SR_TXE
#define TXE_BIT_POS  (uint32_t)7

#define TC_BIT_MASK USART_SR_TC
#define TC_BIT_POS    (uint32_t)6

#define RXNE_BIT_MASK USART_SR_RXNE    
#define RXNE_BIT_POS  (uint32_t)5

#define IDLEIE_BIT_MASK USART_SR_IDLE
#define IDLEIE_BIT_POS  (uint32_t)4

/*
  @brief: Kernel calling handling logic for each UART node
  @input: UART node index in main configuring struct, UART struct 
  @output: void
*/
static void UART_Exception_handling(uint32_t UART_Port_index){

    uint8_t is_TXE_interrupt_u8 = 0;                                                                                        
    uint8_t is_TC_interrupt_u8 = 0;                                                                                         
    uint8_t is_RXE_interrupt_u8 = 0;
    container_info temp_container_conf;
    UART_cfg temp_UART_conf = UART_conf_cst[UART_Port_index];

    //Check whether the interrupt is from TX or RX side
		//TXE flag would be reset by a write into DR register
    is_TXE_interrupt_u8 = ((temp_UART_conf.UART_node)->SR & TXE_BIT_MASK) >> TXE_BIT_POS; 
    is_RXE_interrupt_u8 = ((temp_UART_conf.UART_node)->SR & RXNE_BIT_MASK) >> RXNE_BIT_POS; 

    if(is_TXE_interrupt_u8){
      //Check whether the interrupt is from TXE or TCE
      temp_container_conf = user_string[UART_Port_index];

      //The interrupt is from TCE
      if(temp_container_conf.is_last_byte){
        UART_Transmitt_cplt(UART_Port_index);
      }
      else{ //The interrupt is from TXE
        UART_Transmit_Exception_call(UART_Port_index);
      }
    }

    if(is_RXE_interrupt_u8){
      //interrupt is from RXNE
      UART_Receive_Exception_call(UART_Port_index);
    }
    else{
      is_RXE_interrupt_u8 = ((temp_UART_conf.UART_node)->SR & IDLEIE_BIT_MASK) >> IDLEIE_BIT_POS; 
      if(is_RXE_interrupt_u8){
        UART_Receive_cplt(UART_Port_index);
      }
    }
  return;
}

void USART1_IRQHandler(void){
  UART_Exception_handling(UART_1_TX_idx);                                                                                                  
};


void USART2_IRQHandler(void){
  UART_Exception_handling(0); //dummy
};

void USART3_IRQHandler(void){
  UART_Exception_handling(0); //dummy                                                                                          
};

void UART_Transmit_Exception_call(uint32_t UART_Port_index){
  container_info* temp_container_conf = &(user_string[UART_Port_index]);
  UART_cfg temp_UART_conf = UART_conf_cst[UART_Port_index];

  /*handle if this is the last byte*/                                                                                 
  uint8_t last_byte_u8 = temp_container_conf->container_length - 1;                                       
                                                                                                                        
  if( last_byte_u8 == temp_container_conf->container_current_byte){                                              
    /*last byte, reset TXE interrupt*/                                                                              
    /*set TC interrupt*/                                                                                            
    (temp_UART_conf.UART_node)->CR1 &= ~(TXEIE_EN);                                                                              
    (temp_UART_conf.UART_node)->CR1 |= TCIE_EN;                                                                                  
    //save point to feedback to entry function
    temp_container_conf->is_last_byte = 1;
  }                                                                                                                   

  (temp_UART_conf.UART_node->DR) = temp_container_conf->container_pointer[temp_container_conf->container_current_byte++];       
  
  return;
}

void UART_Receive_Exception_call(uint32_t UART_Port_index){
  //This function would be checked only when a new character is received
  container_info* temp_container_conf = &(user_string[UART_Port_index + NUMB_OF_UART_CONFIGURE]);
  UART_cfg temp_UART_conf = UART_conf_cst[UART_Port_index];

  temp_container_conf->container_pointer[temp_container_conf->container_current_byte++] = temp_UART_conf.UART_node->DR;       
  temp_container_conf->container_length++;
  
  return;
}
