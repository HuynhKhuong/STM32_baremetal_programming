#ifndef __I2C_H__
#define __I2C_H__

#include "I2C_cfg.h"

/* API for Init task*/
void I2C_cfg(uint8_t I2C_port_u8);

/* API for Normal task*/
void I2C_Master_Transmitt_Interrupt(uint8_t I2C_port_u8, uint16_t address_u16, uint8_t* data_string);

#endif
