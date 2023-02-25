#include "I2C.h"


I2C_container_info I2C_user_string[NUM_OF_I2C_NODE];

/*
  @brief: Transmitt as Master using Interrupt mechanism. This function would keep Master constantly transmit data to slave
  @input: I2C_port_u8: index in main configuartion struct, address_u16: address of slave, either in 10-bit or 8-bit mode, uint8_t* data_string
  @appendix:
    1. START condition phase: 
      Setting the START bit causes the interface to generate a Start condition and to switch to Master mode (MSL bit set) when the BUSY bit is cleared.
        Note: In master mode, setting the START bit causes the interface to generate a ReStart condition 
        at the end of the current byte transfer.
      Once the Start condition is sent:
        The SB bit is set by hardware and an interrupt is generated if the ITEVFEN bit is set.

        SB bit is reset by: read SR1 register -> write DR register with the slave address
    
    2. Slave address transmission:
      write DR register with the slave address
      Once address byte is sent, ADDR is set by hardware and an interrupt is generated if the ITEVFEN bit is set.

      ADDR bit is reset by: read SR1 register -> read SR2 register

      Note: in 10 bit mode, first header is sent, ADD10 is set by hardware and an interrupt is generated if the ITEVFEN bit is set.
      ADD10 is reset by: read SR1 registter -> write DR register with the second byte address
      
      Then the Master can decide to enter transmit mode, or receive mode: 
        7-bit mode: LSB (0): transmitter mode, LSB (1): receiver mode
        8-bit mode: header 11110xx0 + slave = transmitter mode, header 11110xx0 + slave + reSTART condition + header 11110xx1 = receiver mode

    3. Master transmission: 
      after step 2: read SR1 register -> read SR2 register

      ADDR bit is reset, TXE is set (TXE = 1 is TX DR buffer empty)
      when TX DR buffer empty, shift register is empty, data would be written directly into shift register

      The master waits until the first data byte is written into I2C_DR 
      When the acknowledge pulse is received, the TxE bit is set by hardware and an interrupt is generated if the ITEVFEN and ITBUFEN bits are set.

    4. Stop master transmission:

      If TxE is set and a data byte was not written in the DR register before the end of the last data
      transmission, BTF is set and the interface waits until BTF is cleared by a read from I2C_SR1
      -> generates a STOP condition

  @output: I2C port would transmit the data string, further design would handle if slave doesn't receive the data (NACK)
*/
void I2C_Master_Transmitt_Interrupt(uint8_t I2C_port_u8, uint16_t address_u16, uint8_t* data_string, uint8_t data_length_u8){
  //Note: This function would handle 7-bit address only

  I2C_cfg temp_I2C_cfg = I2C_Conf_cst[I2C_port_u8];
  uint8_t is_I2C_busy = 0; 

  //Check whether the node is busy or not
  is_I2C_busy = (temp_I2C_cfg.I2C_Node->SR2 & I2C_BUSY_BIT_MASK) >> I2C_BUSY_BIT_POS;

  if(is_I2C_busy) return;

  //First enable Interrupt request 
  temp_I2C_cfg.I2C_Node->CR2 |= I2C_ERR_BIT_MASK; 
  temp_I2C_cfg.I2C_Node->CR2 |= I2C_EVE_BIT_MASK; //currently event bit mask for  and error bit mask
  temp_I2C_cfg.I2C_Node->CR2 |= I2C_BUFFER_BIT_MASK; //interrupt + event bit interrupt -> when TXE event: interrupt happens

  //Store user string
  I2C_user_string[I2C_port_u8].I2C_Node_dir = Comm_dir_Tx;
  I2C_user_string[I2C_port_u8].container_length = data_length_u8;
  I2C_user_string[I2C_port_u8].container_current_byte = 0;
  I2C_user_string[I2C_port_u8].is_last_byte = 0;
  I2C_user_string[I2C_port_u8].master_address = temp_I2C_cfg.I2C_Node_adress.Address_u16;
  I2C_user_string[I2C_port_u8].slave_address = address_u16;

  for(int i = 0; i  < data_length_u8; i++){
   I2C_user_string[I2C_port_u8].container_pointer[i] = data_string[i];   
  }

  //Generate START BIT
  temp_I2C_cfg.I2C_Node->CR1 |= I2C_START_BIT_MASK;
  
  return;
}

/// @brief: Read a slave data as a Master using Interrupt mechanism. 
//          This function would trigger Master transmit address to slave, causing slave to send back data to master
/// @param I2C_port_u8: index in main configuartion struct,   
/// @param address_u16: address of slave, either in 10-bit or 8-bit mode,
/// @param data_string: container of the slave's data 

void I2C_Master_Read_Interrupt(uint8_t I2C_port_u8, uint16_t address_u16, uint8_t* data_string){

  //dummy return, this function would be handled later
  return;
}

