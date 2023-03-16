#ifndef __NVIC_PRI_H__
#define __NVIC_PRI_H__

#include "NVIC_pri_cfg.h"

/// APIs for config task
void Excp_pri_cfg(void);

/// APIs public for user
void Set_Interrupt_prio(uint8_t interrupt_instance, uint32_t pre_emption_val_u32, uint32_t sub_priority_val_u32);
uint32_t Get_Interrupt_prio(uint8_t interrupt_instance);


#endif
