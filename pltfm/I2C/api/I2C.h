#ifndef __I2C_H__
#define __I2C_H__

#include "I2C_cfg.h"

/* API for Init task*/
void I2C_Init(void);

/* API for Normal task*/
/*
  Different from UART, a Master can handle 2 communication directions:
  Tx and Rx:

  a Master after sending out Slave address can become a data transmitter or a data receiver
*/


void I2C_Master_Transmitt_Interrupt(uint8_t I2C_port_u8, uint16_t address_u16, uint8_t* data_string, uint8_t data_length_u8);
void I2C_Master_Read_Interrupt(uint8_t I2C_port_u8, uint16_t address_u16, uint8_t* data_string);

#endif
