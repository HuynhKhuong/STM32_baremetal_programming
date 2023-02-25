#include "I2C_ex.h"

/**************************************
  @author: Huynh Khuong
  This file defines exception handling for I2C peripherals
**************************************/


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

  //As master transmits with a sequence of interrupts, with an interrupt detected, there are no previous interrupt (in the sequence) could occur
  ///Handle as SB interrupt
  if(is_SB_interrupt){
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
    /// Leave blank, would be handled later
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
