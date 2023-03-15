#ifndef __NVIC_PRI_CFG_H__
#define __NVIC_PRI_CFG_H__

#include "stm32f10x.h"

/***********************
@Author: Huynh Khuong

Define structure to configure priorities of interrupts registered by user
*************************/

#define NUM_OF_INTERRUPTS_REGISTERED     (uint8_t)3 ///

typedef enum{
  PRIGROUP_FOUR_ZERO = (uint32_t)0, //pre-emption priority = 4 bits, sub-priority = 0 bit
  PRIGROUP_THREE_ONE = (uint32_t)1, 
  PRIGROUP_TWO_TWO = (uint32_t)2, 
  PRIGROUP_ONE_THREE = (uint32_t)3, 
  PRIGROUP_ZERO_FOUR = (uint32_t)4,  
}pri_group_div;

typedef struct{
  IRQn_Type interrupt_instance;
  uint32_t pre_emption;
  uint32_t sub_priority;
}Interrupt_prio_setup;

extern const Interrupt_prio_setup interrupt_priority_conf[NUM_OF_INTERRUPTS_REGISTERED];

#endif 
