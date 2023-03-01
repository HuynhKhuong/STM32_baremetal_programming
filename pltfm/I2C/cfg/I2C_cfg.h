#ifndef __I2C_CFG_H__
#define __I2C_CFG_H__

#include "stm32f10x.h"

/***********************
@Author: Huynh Khuong
Define structures to add configurations to I2C peripherals as a master transmission
As this section would configure STM32 as a basic manner, there is no error handling, no DMA feature

Advanced feature would be added in the future
*************************/

//Shared MACROS for internal and external using
#define I2C_BUSY_BIT_MASK I2C_SR2_BUSY
#define I2C_BUSY_BIT_POS  (uint32_t)1

#define I2C_ERR_BIT_MASK  I2C_CR2_ITERREN
#define I2C_EVE_BIT_MASK  I2C_CR2_ITEVTEN
#define I2C_BUFFER_BIT_MASK I2C_CR2_ITBUFEN

#define I2C_START_BIT_MASK I2C_CR1_START
#define I2C_STOP_BIT_MASK  I2C_CR1_STOP
#define I2C_ACK_STATUS_BIT_MASK I2C_CR1_ACK

#define I2C_SB_STATUS_BIT_MASK I2C_SR1_SB
#define I2C_ADD_STATUS_BIT_MASK  I2C_SR1_ADDR
#define I2C_TXE_STATUS_BIT_MASK I2C_SR1_TXE
#define I2C_RXNE_STATUS_BIT_MASK I2C_SR1_RXNE
#define I2C_BTF_STATUS_BIT_MASK I2C_SR1_BTF

#define NUM_OF_I2C_NODE     (uint8_t)1
#define TOTAL_PIN_PER_I2C   (uint8_t)2

extern uint8_t  is_I2C1_NVIC_registered; 
extern uint8_t  is_I2C2_NVIC_registered;

//Member enum configuring RCC source clock
typedef enum{
  RCC_I2C1_EN = RCC_APB1ENR_I2C1EN,
  RCC_I2C2_EN = RCC_APB1ENR_I2C2EN,
}RCC_enable_I2C;

//Member struct configuring RCC configuration for I2C
typedef struct{
  RCC_enable_I2C I2C_Node_enable;
  volatile uint32_t *Clock_cfg_reg;
}I2C_clock_src_config;

//Member struct configuring SCL clock generation for Master
typedef struct{
  float RCC_Freq_Mhz;  
  float SCL_clock_Mhz; 
}SCL_clock_Config;

//Member enum define I2C role: master or slave
typedef enum{
  SLAVE = 0,
  MASTER = 1,
}I2C_Role;

//Member enum define Address Mode
typedef enum{
  SEVEN_BIT_ADDR = (uint16_t)0,
  TEN_BIT_ADDR = I2C_OAR1_ADDMODE,
}Address_mode;

//Member struct configuring a node address
typedef struct{
  Address_mode Address_length;
  uint16_t Address_u16;
  volatile uint16_t* Address_config_reg;
}Address_config;

//Member enum define speed mode
typedef enum{
  SM_MODE =  (uint16_t)0,
  FM_MODE =   I2C_CCR_FS, //Operation for FS is not support currently
}Speed_Mode;


//Relating to Data Reception layout
typedef enum{
  ACK_OF_CURRENT_BYTE = 0,
  ACK_OF_NEXT_BYTE = I2C_CR1_POS,
}ACK_BIT_POS;

//Main struct configuring I2C 
typedef struct{
  /*RCC configuration*/
  I2C_clock_src_config I2C_RCC_config;
  I2C_TypeDef*  I2C_Node;

  //Variants configuring
  I2C_Role I2C_Node_role;
  Address_config I2C_Node_adress;
  Speed_Mode I2C_Speed_Mode;

  //Clock on SCL pin configuration
  SCL_clock_Config SCL_desired_clock;

  //GPIO Check 
  uint8_t pin[TOTAL_PIN_PER_I2C]; 

  //Interrupt register
  uint8_t* is_I2C_node_NVIC_registered;

  IRQn_Type NVIC_index_1; //index 1 for event interrupt
  IRQn_Type NVIC_index_2; //index 2 for error interrupt
}I2C_cfg;

extern const I2C_cfg I2C_Conf_cst[NUM_OF_I2C_NODE];

#endif
