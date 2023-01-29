#ifndef __RCC_EX_H__
#define __RCC_EX_H__

#include "stm32f10x.h"

//declare wrapper which would be called in pre-defined related-RCC exception handler: Systick timer 
uint8_t Systick_Callback(void);

#endif 
