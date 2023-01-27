#ifndef __RCC_CFG_H__
#define __RCC_CFG_H__

#include "stm32f10x.h"

//Declare struct to configure clock tree
typedef struct{
  uint32_t dummy_value;
}RCC_conf;

//member struct configuring systick
typedef struct{
  uint32_t dummy_value;
}SysTick_Conf;

//member struct configuring PLL clock
typedef struct{
  uint32_t dummy_value;
}PLL_Conf;

//member enum configuring PLL_source 
typedef enum{
  HSI = 0,  
  HSE = 1
}PLL_source;

//member enum configuring SYS_source 
typedef enum{
  dummy_value_1 = 0
}SYS_source;

//member enum configuring AHB division 
typedef enum{
  dummy_value_2 = 0
}AHB_division;

//member enum configuring APB division
typedef enum{
  dummy_value_3 = 0
}APB_division;

/*
  commentary section showing desired OUTPUT: 
  SYSclock freq, AHB freq, APB freq

*/

#endif 
