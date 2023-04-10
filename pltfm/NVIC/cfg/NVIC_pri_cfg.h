#ifndef __NVIC_PRI_CFG_H__
#define __NVIC_PRI_CFG_H__

#include "stm32f10x.h"

/***********************
@Author: Huynh Khuong

Define structure to configure priorities of interrupts registered by user
*************************/

#define GET_INTERRUPT_REGISTERED_STATUS(Interrupt_index)                        __NVIC_GetEnableIRQ(Interrupt_index)
#define SET_INTERRUPT_REGISTERED_PRIORITY(Interrupt_index, Interrupt_priority)  __NVIC_SetPriority(Interrupt_index, Interrupt_priority)
#define SET_PRIORITYGROUP(group_value)                                          __NVIC_SetPriorityGrouping(group_value) 
#define DISABLE_INTERRUPT(interrupt_instance)     															__NVIC_DisableIRQ(interrupt_instance)
#define ENABLE_INTERRUPT(interrupt_instance)      															__NVIC_EnableIRQ(interrupt_instance)

#define MAX_NUMB_PRE_EMPTION_BIT  (uint32_t)4

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

extern Interrupt_prio_setup interrupt_priority_conf[NUM_OF_INTERRUPTS_REGISTERED];
extern const pri_group_div global_pri_group;

#endif 
