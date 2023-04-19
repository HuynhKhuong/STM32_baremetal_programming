#ifndef __GPIO_H__
#define __GPIO_H__

#include "GPIO_cfg.h"

#define User_define_EXTI_Callback

//APIs for Init task
void GPIO_Init(void);

//APIs for Normal task
void GPIO_WritePin(uint32_t pin_index, uint32_t state); //Available for GPO only
uint32_t GPIO_ReadPin(uint32_t pin_index); //Available for GPI only
uint32_t is_pin_configured(uint32_t pin_index); //API for other component to check whether the configuration is done or not

//An internal function helping to re-map pin index to its position in BSRR register
uint32_t BSRR_Remapping(uint32_t PIN, uint32_t bit_state);

//AIPs for EXTI related task
void GPIO_EXT_disable(uint32_t pin_index); //disable the interrupt pin

///User-define logic after the communication is done
#ifdef User_define_EXTI_Callback
void EXTI_0_CallBack(void);
///User can define more callback corresponding to other lines
#endif

#endif 
