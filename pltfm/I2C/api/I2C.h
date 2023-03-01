#ifndef __I2C_H__
#define __I2C_H__

#include "I2C_cfg.h"

#undef User_define_I2C_logic

//member enum used specifically for user's string only, would be handled in interrupt
typedef enum{
  Comm_dir_Tx = (uint8_t)0,
  Comm_dir_Rx = (uint8_t)1,
}Comm_dir;

/* API for Init task*/
void I2C_Init(void);

/* API for Normal task*/
/*
  Different from UART, a Master can handle 2 communication directions:
  Tx and Rx:

  a Master after sending out Slave address can become a data transmitter or a data receiver
*/


void I2C_Master_Transmitt_Interrupt(uint8_t I2C_port_u8, uint16_t address_u16, uint8_t* data_string, uint8_t data_length_u8);
void I2C_Master_Read_Interrupt(uint8_t I2C_port_u8, uint16_t address_u16, uint8_t* data_string, uint8_t data_length);


/*API for Interrupt task*/
//This function is visible to user, would be invoked when the whole transmission is done
void I2C_Master_Transmitt_cplt(uint8_t I2C_port_u8);
void I2C_Master_Receive_cplt(uint8_t I2C_port_u8);

///User-define logic 
#ifdef User_define_I2C_logic
void I2C_Master_TX_GP_callback(void);
void I2C_Master_RX_GP_callback(void);
#endif


//Member struct containing user's string
typedef struct{
  uint8_t  container_pointer[100];
  uint32_t container_length;
  uint32_t container_current_byte;
  uint32_t is_last_byte;
  uint8_t* user_provided_string; //This pointer is for RX only

  uint16_t slave_address; //for slave 
  uint16_t master_address; //for master
  Comm_dir I2C_Node_dir; //which Rx or Tx
}I2C_container_info;

extern I2C_container_info I2C_user_string[NUM_OF_I2C_NODE];

#endif
