#ifndef __NVIC_PRI_CFG_H__
#define __NVIC_PRI_CFG_H__

#include "stm32f10x.h"

/***********************
@Author: Huynh Khuong

Define structure to configure priorities of interrupts registered by user
*************************/

#define NUM_OF_INTERRUPTS_REGISTERED     (uint8_t)3 ///

typedef enum{
  PRIGROUP_SEVEN_ONE = (uint32_t)0, //pre-emption priority = 7 bits, sub-priority = 1 bit
  PRIGROUP_SIX_TWO = (uint32_t)1, 
  PRIGROUP_FIVE_THREE = (uint32_t)2, 
  PRIGROUP_FOUR_FOUR = (uint32_t)3, 
  PRIGROUP_THREE_FIVE = (uint32_t)4,  
  PRIGROUP_TWO_SIX = (uint32_t)5, 
  PRIGROUP_ONE_SEVEN = (uint32_t)6, 
  PRIGROUP_ZERO_EIGHT = (uint32_t)7, 
}pri_group_div;

typedef struct{
  IRQn_Type interrupt_instance;
  uint32_t pre_emption;
  uint32_t sub_priority;
}Interrupt_prio_setup;

extern const Interrupt_prio_setup interrupt_priority_conf[NUM_OF_INTERRUPTS_REGISTERED];

#endif 
