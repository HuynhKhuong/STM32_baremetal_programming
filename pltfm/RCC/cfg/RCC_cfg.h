#ifndef __RCC_CFG_H__
#define __RCC_CFG_H__

#include "stm32f10x.h"

//Declare struct to configure clock tree

#define NUM_OF_CLKTREE_INSTANCE 1

//member enum configuring PLL_multiplier
typedef enum{
  PLLx4 = (uint8_t)2,
  PLLx5 = (uint8_t)3,
  PLLx6 = (uint8_t)4,
  PLLx7 = (uint8_t)5,
  PLLx8 = (uint8_t)6,
  PLLx9 = (uint8_t)7,
  PLLx6_5 = (uint8_t)13,
}PLL_Mul;

//member enum configuring PLL_source 
typedef enum{
  PLL_HSI = 0,  // HSI/2
  PLL_HSE = 1,   // HSE/1
  PLL_HSE_DIV2 = 2, //HSE/2
}PLL_source;

//member struct configuring PLL clock
typedef struct{
  PLL_source PLL_Clock_src;
  PLL_Mul Multiplier;
}PLL_Conf;

//member enum configuring SYS_source 
typedef enum{
  SYS_HSI = 0,
  SYS_HSE = 1, 
  SYS_PLL = 2
}SYS_source;

//member enum configuring AHB division 
typedef enum{
  AHB_NO_DIVISION = 0,
  AHB_DIV_2 = 8, 
  AHB_DIV_4, 
  AHB_DIV_8, 
  AHB_DIV_16, 
  AHB_DIV_32, 
  AHB_DIV_64, 
  AHB_DIV_128, 
  AHB_DIV_256, 
  AHB_DIV_512, 
}AHB_division;

//member enum configuring APB division
typedef enum{
  APB_NO_DIVISION = 0,
  APB_DIV_2 = 4, 
  APB_DIV_4, 
  APB_DIV_8, 
  APB_DIV_16, 
}APB_division;

/*
  Arm Cortex M3 CPU has its own 24-bit timer which is called System_timer/systick
  That counts down from the reload value to zero, reloads the value of SYST_RVR register on the next clock edge

  There are 4 registers to manipulate the Systick:
  - SYST_CSR: Control & Status Register
  - SYST_RVR: Reload value register
  - SYST_CVR: Current value register
  - SYST_CALIB: Don't pay attention 
  
  Clock source for Systick are Core-clock coming from AHB bus clock and external core-clock coming from AHB bus clock/8 (both are eeded by RCC)
  
  To configure the reload value, we have to align with the frequency of the RCC's clock source

  For more info: Read Arm Cortex M3 manual reference

  //Interrupt configuration for systick

  To disable systick interrupt, use PRIMASK register. It would prevent any exceptions with configurable priority from being active

  Systick register locates in 'core_cm3.h'
*/

//main struct
typedef struct{
  //Configuration registers
  volatile uint32_t *clock_src_control_reg;
  volatile uint32_t *clock_config_reg;
  //PLL Configuration
  PLL_Conf clock_src_PLL; //config if use PLL as source, leave dummy value if don't use
  SYS_source systick_clock_src; //Clock source for System tick
  //Pre-scaler for peripherals' buses
  AHB_division AHB_prescaler;
  APB_division APB1_prescaler;
  APB_division APB2_prescaler;
  //Systick configuration
  uint32_t Systick_reload_val_u32;
}RCC_conf;


extern const RCC_conf clock_tree_conf_cst[NUM_OF_CLKTREE_INSTANCE];

#endif 
