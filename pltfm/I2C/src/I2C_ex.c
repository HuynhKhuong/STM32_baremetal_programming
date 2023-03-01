#include "I2C_ex.h"

/**************************************
  @author: Huynh Khuong
  This file defines exception handling for I2C peripherals
**************************************/
/// local define supporting I2C reception
#define MASTER_RX_3BYTES_LEFT     (uint8_t)2
#define MASTER_RX_2BYTES_LEFT     (uint8_t)1
#define MASTER_RX_1BYTES_LEFT     (uint8_t)0

/*
  @brief: Kernel calling handling logic for I2C node as master
  @input: I2C node index in main configuring struct 
  @output: void
*/
static void I2C_Master_Exception_handling(uint8_t I2C_Port_index){
  /*Exception would handle Master as 3 phases 
    1. SB phase
    2. Address phase
    3. Transmit/Receive phase (one byte each)
    4. Communication success
  */

  I2C_cfg temp_I2C_conf = I2C_Conf_cst[I2C_Port_index];
  I2C_container_info temp_container = I2C_user_string[I2C_Port_index];
  uint8_t is_Master_TX = (temp_container.I2C_Node_dir == Comm_dir_Tx);

  const uint16_t current_status = temp_I2C_conf.I2C_Node->SR1;

  const uint8_t is_SB_interrupt = (current_status) & I2C_SB_STATUS_BIT_MASK; 
  const uint8_t is_ADD_interrupt = (current_status)& I2C_ADD_STATUS_BIT_MASK;
  const uint8_t is_BYTE_interrupt = (current_status) & I2C_TXE_STATUS_BIT_MASK;
  const uint8_t is_BYTE_FINISH_interrupt = ((current_status) & I2C_BTF_STATUS_BIT_MASK) && (temp_container.is_last_byte);
  const uint8_t is_BYTE_Recep_interrupt = ((current_status) & I2C_RXNE_STATUS_BIT_MASK)||((current_status) & I2C_BTF_STATUS_BIT_MASK);
  const uint8_t is_BYTE_FINISH_Recep_interrupt = ((current_status) & I2C_RXNE_STATUS_BIT_MASK) && (temp_container.is_last_byte);

  //As master transmits/receives with a sequence of interrupts, with an interrupt detected, there are no previous interrupt (in the sequence) could occur
  ///Handle as SB interrupt
  if(is_SB_interrupt){ // > 0 : true; //= 0 : false
    I2C_SB_Exception_call(I2C_Port_index, is_Master_TX);
    return;
  }

  ///Handle as ADD interrupt
  if(is_ADD_interrupt){
    I2C_ADD_Exception_call(I2C_Port_index);
    return;
  }

  if(is_Master_TX){
		///Call final function, visible to user
    if(is_BYTE_FINISH_interrupt){
      I2C_Master_Transmitt_cplt(I2C_Port_index);
      return;
    }
		
    ///Handle with each byte transmitted interrupt
    if(is_BYTE_interrupt){
      I2C_BYTE_TRANSMITTED_Exception_call(I2C_Port_index);
      return;
    }


  }
  else{ ///Is_Master_RX
    ///The receiving strategy is based on approach 2 in user manual
    //last byte received
    if(is_BYTE_FINISH_Recep_interrupt){
      I2C_Master_Receive_cplt(I2C_Port_index);
      return;
    }

    //middle bytes received
    if(is_BYTE_Recep_interrupt){ //handle in case multiple byte reception (N > 2)
      I2C_BYTE_RECEIVED_Exception_call(I2C_Port_index);
      return;
    }
  }
}

/*
  @brief: Kernel calling handling logic for each I2C node
  @input: I2C node index in main configuring struct 
  @output: void
*/
static void I2C_Exception_handling(uint8_t I2C_Port_index){
  /// Exception handling would be based on the I2C Port role to handle
  ///
    uint8_t is_Node_MASTER = 0;

    I2C_cfg temp_I2C_conf = I2C_Conf_cst[I2C_Port_index];
    is_Node_MASTER = (temp_I2C_conf.I2C_Node_role == MASTER);

    if(is_Node_MASTER){
      I2C_Master_Exception_handling(I2C_Port_index);
    }
    else{
      /// This section would call logic for slave handling
    }

  return;
}


/// @brief Exception handler defined by hardware specific for I2C
/// @param void

void I2C1_EV_IRQHandler(void){

  const uint8_t I2C1_index = 0;
  I2C_Exception_handling(I2C1_index);
  return;
}

void I2C1_ER_IRQHandler(void){
  ///Dummy function, would be handled later
}

void I2C2_EV_IRQHandler(void){
  const uint8_t I2C2_index = 0; //haven't configured for I2C2 yet, leave dummy index
  I2C_Exception_handling(I2C2_index );
  return;
}

void I2C2_ER_IRQHandler(void){
  ///Dummy function, would be handled later
}

/// @brief Define logic handle for master when SB has been started
/// @param I2C_Port_index 
/// @return void
void I2C_SB_Exception_call(uint8_t I2C_Port_index, uint8_t is_dir_TX){

  I2C_cfg temp_I2C_conf = I2C_Conf_cst[I2C_Port_index];
  I2C_container_info temp_container = I2C_user_string[I2C_Port_index];
  uint8_t slave_address  = temp_container.slave_address;

  ///SR1 has been read, another write to DR register would clear the SB bit in SR1
  // Prepare Address, This address would be configured based on the Comm_Dir

  slave_address <<= 1;
  slave_address = (is_dir_TX)?(slave_address|0):(slave_address | 1);

  temp_I2C_conf.I2C_Node->DR = slave_address;
}

/// @brief Define logic handle for master when ADD has been transmitted
/// @param I2C_Port_index 
/// @return void
void I2C_ADD_Exception_call(uint8_t I2C_Port_index){
  I2C_cfg temp_I2C_conf = I2C_Conf_cst[I2C_Port_index];

  uint16_t SR2_status_u16 = 0;

  ///SR1 has been read, another read to SR2 would clear the ADD bit in SR1
  ///When ADDR bit is reset, Shift register & DR is empty, only TXE bit is set, not BTF 
  ///Which occurs a byte-transmitted-exception call
  SR2_status_u16 = temp_I2C_conf.I2C_Node->SR2;
}

/// @brief Define logic handle for master when a single byte (except the last byte) is transmitted
/// @param I2C_Port_index
/// @return void

void I2C_BYTE_TRANSMITTED_Exception_call(uint8_t I2C_Port_index){

  I2C_cfg temp_I2C_conf = I2C_Conf_cst[I2C_Port_index];
  I2C_container_info* temp_container = &(I2C_user_string[I2C_Port_index]);

  /*handle if this is the last byte*/                                                                                 
  uint8_t last_byte_u8 = temp_container->container_length - 1;                                       

  if( last_byte_u8 == temp_container->container_current_byte){                                              
    /*last byte, reset TXE interrupt*/                                                                              
    /*set BTF interrupt*/                                                                                            
    (temp_I2C_conf.I2C_Node)->CR2 &= ~(I2C_BUFFER_BIT_MASK);

    //save point to feedback to entry function
    temp_container->is_last_byte = 1;
  }                                                                                                                   
  ///This bit is cleared by writing data in to DR 
  (temp_I2C_conf.I2C_Node->DR) = temp_container->container_pointer[temp_container->container_current_byte++];       
}

/// @brief Define logic handle for master when the last byte is transmitted
/// @param I2C_Port_index
/// @return void

void I2C_Master_Transmitt_cplt(uint8_t I2C_port_u8){

  I2C_cfg temp_I2C_conf = I2C_Conf_cst[I2C_port_u8];

  ///disable all interrupt (except error interrupt)
  (temp_I2C_conf.I2C_Node)->CR2 &= ~(I2C_BUFFER_BIT_MASK);
  (temp_I2C_conf.I2C_Node)->CR2 &= ~(I2C_EVE_BIT_MASK);

  ///Trigger stop condition would clear both TxE and BTF flag
  (temp_I2C_conf.I2C_Node)->CR1 |= I2C_STOP_BIT_MASK;

  ///User-define logic section
  #ifdef User_define_I2C_logic
    I2C_Master_TX_GP_callback(); 
  #endif 
}

/// @brief This function handles bytes reception from first byte to the 2nd last byte following the approach 2 method described in STM32 manual
/// @param I2C_Port_index 
/// @return void

void I2C_BYTE_RECEIVED_Exception_call(uint8_t I2C_Port_index){
  // This function would continously receive bytes until there are 3 bytes left to be received

  I2C_cfg temp_I2C_conf = I2C_Conf_cst[I2C_Port_index];
  I2C_container_info* temp_container = &(I2C_user_string[I2C_Port_index]);
  
  uint8_t num_of_byte_left_u8 = (temp_container->container_length) - (temp_container->container_current_byte) - 1;

  // Different strategy based on the number of byte left in reception
  if(MASTER_RX_3BYTES_LEFT == num_of_byte_left_u8){
    //Disable RxNE interrupt
    temp_I2C_conf.I2C_Node->CR2 &= ~I2C_BUFFER_BIT_MASK;
		temp_container->container_current_byte++;
    //Take no action, wait for 3rd last byte in DR register & 2nd last byte in Shift register
    //On 2nd last byte reception, BTF would trigger the reception, //stretch the SCL line down
  }
  else if(MASTER_RX_2BYTES_LEFT == num_of_byte_left_u8){
    //Take 3rd last byte
    //Trigger NACK
    temp_I2C_conf.I2C_Node->CR1 &= ~I2C_ACK_STATUS_BIT_MASK;
    temp_container->container_pointer[(temp_container->container_current_byte-1)] = temp_I2C_conf.I2C_Node->DR;
    //A read to SR1 register -> read DR register -> clear BTF -> continue reception
		temp_container->container_current_byte++;
  }
  else if(MASTER_RX_1BYTES_LEFT == num_of_byte_left_u8){
    //Take 2nd last byte

    //Configured RXnE condition to receive the last byte
    (temp_I2C_conf.I2C_Node)->CR2 |= I2C_BUFFER_BIT_MASK;
    //Configured STOP condition
    (temp_I2C_conf.I2C_Node)->CR1 |= I2C_STOP_BIT_MASK;

    temp_container->container_pointer[temp_container->container_current_byte-1] = temp_I2C_conf.I2C_Node->DR;
    temp_container->is_last_byte = 1;
  }
  else{ //middle bytes
    temp_container->container_pointer[temp_container->container_current_byte++] = temp_I2C_conf.I2C_Node->DR;
  }
  return;
}

/// @brief Function hanlding the last byte reception, would be visible to user
/// @param I2C_port_u8 
/// @return void
void I2C_Master_Receive_cplt(uint8_t I2C_port_u8){
  
  I2C_cfg temp_I2C_conf = I2C_Conf_cst[I2C_port_u8];
  I2C_container_info* temp_container = &(I2C_user_string[I2C_port_u8]);
  ///disable all interrupt (except error interrupt)

  (temp_I2C_conf.I2C_Node)->CR2 &= ~(I2C_BUFFER_BIT_MASK);
  (temp_I2C_conf.I2C_Node)->CR2 &= ~(I2C_EVE_BIT_MASK);
	temp_container->container_pointer[temp_container->container_current_byte] = temp_I2C_conf.I2C_Node->DR;
	
	//return to user's string

  for(int i = 0; i < temp_container->container_length; i++){
    temp_container->user_provided_string[i] = temp_container->container_pointer[i];
  }
  ///User-define logic section
  #ifdef User_define_I2C_logic
    I2C_Master_RX_GP_callback(); 
  #endif 

  return;
}
