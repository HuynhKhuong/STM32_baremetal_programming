#include "I2C.h"

//APIs for Exception task
//These function would handle interrupt call for master after each phase (In 7bit address mode)
void I2C_SB_Exception_call(uint8_t I2C_Port_index, uint8_t is_dir_TX);
void I2C_ADD_Exception_call(uint8_t I2C_Port_index);
void I2C_BYTE_TRANSMITTED_Exception_call(uint8_t I2C_Port_index);
void I2C_BYTE_RECEIVED_Exception_call(uint8_t I2C_Port_index);
