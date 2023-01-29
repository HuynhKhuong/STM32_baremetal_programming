#ifndef __GPIO_CFG_H__
#define __GPIO_CFG_H__

/************************
@Author: Huynh Khuong
Define structures to add configurations to GPIO peripherals
*************************/
#include "stm32f10x.h"

//A struct containing a pin level configuration

//The procedure includes 2 steps: 
//1. RCC configuration
//2. Pin configuration (GPIO registers)

#define NUM_OF_PINS_CONFIGURE         (uint32_t)2
//Because the configuration is for pin level, Some configurations for port level are common and no need to 
//repeat the some steps. Flags are used to indicate whether the configuration is done before or not 
extern uint8_t is_portA_configured;
extern uint8_t is_portB_configured;
extern uint8_t is_portC_configured;
extern uint8_t is_portD_configured;

//Member enum specified for RCC configuration for GPIO Port
typedef enum{
  PORT_A_Enable_bit = RCC_APB2ENR_IOPAEN,
  PORT_B_Enable_bit = RCC_APB2ENR_IOPBEN, 
  PORT_C_Enable_bit = RCC_APB2ENR_IOPCEN,                  
  PORT_D_Enable_bit = RCC_APB2ENR_IOPDEN                
}GPIO_RCC_conf;


//Member struct used for configurating RCC for GPIO Port
typedef struct{
  volatile uint32_t* Conf_reg_u32; //register used for configuring RCC for GPIO Port
  GPIO_RCC_conf Conf_bit_u32; //bit position to turn on/off for a GPIO Port's RCC configuration
}GPIO_RCC;

//Member enum to identify Input configuration
typedef enum{
  ANALOG = 0,
  FLOATING_INPUT = 1,
  PUPD_INPUT = 2,
  NOT_INPUT = 3
}GPIO_Input_CNF;

//Member enum to identify Output configuration
typedef enum{
  OUTPUT_PP = 0,
  OUTPUT_OD = 1, //Open-drain
  AL_OUTPUT_PP = 2, //Alternative function
  AL_OUTPUT_OD = 3, //Alternative function
  NOT_OUTPUT = 4
}GPIO_Output_CNF;

//Member enum to identify OUTPUT & INPUT
typedef enum{
  INPUT_MODE = 0,
  OUTPUT_MODE_10MHz = 1,
  OUTPUT_MODE_2MHz = 2,
  OUTPUT_MODE_50MHz = 3,
}GPIO_MODE;

//Member enum defining pins index
typedef enum{
  PIN_0 ,
  PIN_1 ,
  PIN_2 ,
  PIN_3 ,
  PIN_4 ,
  PIN_5 ,
  PIN_6 ,
  PIN_7 ,
  PIN_8 ,
  PIN_9 ,
  PIN_10 ,
  PIN_11 ,
  PIN_12 ,
  PIN_13 ,
  PIN_14 ,
  PIN_15 ,
  PIN_16 ,
  PIN_17 ,
  PIN_18 ,
  PIN_19 ,
  PIN_20 ,
  PIN_21 ,
  PIN_22 ,
  PIN_23 ,
  PIN_24 ,
  PIN_25 ,
  PIN_26 ,
  PIN_27 ,
  PIN_28 ,
  PIN_29 ,
  PIN_30 ,
  PIN_31 ,
  NUM_OF_PINS,
}PIN_LIST;

//Main struct
typedef struct{
  //RCC configuration
  GPIO_RCC GPIO_Port_RCC_cfg_str;
  //Pin configuration
  GPIO_TypeDef* GPIO_Port; // Port: Which port does the pin belongs to 
  uint8_t *is_port_configured; //check global variables whether the port is configured or not
  PIN_LIST PIN_Index; //Which pin is it
  uint8_t is_Reg_CRL; //CRL or CRH
  GPIO_MODE Pin_Mode; //Input or output 
  GPIO_Input_CNF  Pin_Input_cnf; //Left = NOT_INPUT if this pin is output 
  GPIO_Output_CNF Pin_Output_cnf;//Left = NOT_OUTPUT if this pin is input

}GPIO_conf;

/*-------------------------------------------------------------------------*/
extern const GPIO_conf GPIO_conf_cst[NUM_OF_PINS_CONFIGURE];

#endif 
