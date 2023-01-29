#ifndef __RCC_H__
#define __RCC_H__

#include "RCC_cfg.h"
#include "RCC_ex.h"
//APIs for Init Task
void Clock_tree_init(uint8_t clock_tree_profile);

//APIs for NOrmal Task in combination with Exception task: delay function
void Systick_Delay(uint32_t ms_delay_u32);


#endif
