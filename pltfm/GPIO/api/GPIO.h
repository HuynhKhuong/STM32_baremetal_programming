#ifndef __GPIO_H__
#define __GPIO_H__

#include "GPIO_cfg.h"

//APIs for Init task
void GPIO_Init(void);

//APIs for Normal task
void GPIO_WritePin(uint32_t pin_index, uint32_t state); //Available for GPO only
uint32_t GPIO_ReadPin(uint32_t pin_index); //Available for GPI only
uint32_t is_pin_configured(uint32_t pin_index); //API for other component to check whether the configuration is done or not

#endif 
