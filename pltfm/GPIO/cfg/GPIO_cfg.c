#include "GPIO.h"
#include <stdio.h>

/************************
@Author: Huynh Khuong
Configure structures to add configurations to GPIO peripherals
Configuring logic to init GPIO peripherals 
*************************/

//flags indicating port status
uint8_t is_portA_configured = 0;
uint8_t is_portB_configured = 0;
uint8_t is_portC_configured = 0;
uint8_t is_portD_configured = 0;

//struct used to configure GPIO on PIN level
const GPIO_conf GPIO_conf_cst[NUM_OF_PINS_CONFIGURE] = {
//1. PIN 13C, OUTPUT PUSH_PULL
{ 
  /*RCC configuration*/
  {
    &(RCC->APB2ENR), //RCC reg
    PORT_C_Enable_bit
  },
  GPIOC,
  &is_portC_configured,
  PIN_13,
  0, //reg CRH not reg CRL
  OUTPUT_MODE_10MHz, 
  NOT_INPUT,
  OUTPUT_PP
},

//2. PIN 1A, INPUT PULL_UP, PULL_DOWN
{
  {
    &(RCC->APB2ENR), //RCC reg
    PORT_A_Enable_bit
  },
  GPIOA,
  &is_portA_configured,
  PIN_1,
  1, //reg CRL not CRH
  INPUT_MODE,
  PUPD_INPUT,
  NOT_OUTPUT
}
};

//Mapping from cnf/mode enum to bit position
#define BIT_POS_SHIFT   (uint32_t)4 

/*
  @brief: A kernel mapping bits to CRH/CRL GPIO register
  @input: PIN index (0->31), BITS value (ranges from 0-15, depends on what CNF and MODE values are configured for that pin)
  @output: values which are mapped to the pin position in that register
*/
static uint32_t CR_reg_bit_mapping(uint32_t PIN, uint32_t BITS) {
  //STM32 CR registers are devided into 2 regs: CRH & CRL each supports 8 pins only
  //In Author's definition, pins are described from 0 -> 31
  //Their index should be remapped into 8-pin groups before being mapped into CR registers
  uint32_t pin_group_mapping_u32 = (PIN%8);
  return (uint32_t)(((uint32_t)0 | BITS) << (pin_group_mapping_u32*BIT_POS_SHIFT));
}

/*
  @brief: Init function to Initiate GPIO peripherals 
  @input: Configuring struct (pre-defined global struct)
  @output: void, the GPIO is configured
*/
void GPIO_Init(void){

  GPIO_conf temp_pin_conf_str;
  uint8_t CNF_MODE_bits_u8 = 0;
  uint32_t CNF_MODE_bits_mapped_u32 = 0;
  uint32_t pins_position_mapped_u32 = 0;
  volatile uint32_t *CR_reg_u32 = NULL;
  //Iterate through all pin configuration
  for(uint8_t i = 0; i < NUM_OF_PINS_CONFIGURE; i++){
    temp_pin_conf_str = GPIO_conf_cst[i];

    //check pre-condition to configure port RCC
    switch(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_bit_u32){
      case PORT_A_Enable_bit: 
        if(!is_portA_configured){
          is_portA_configured = 1; //Only configure port RCC once 
          *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) &= ~(PORT_A_Enable_bit); 
          *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) |= PORT_A_Enable_bit;
        }
        break;
      case PORT_B_Enable_bit:
        if(!is_portB_configured){
          is_portB_configured = 1; //Only configure port RCC once 
          *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) &= ~(PORT_B_Enable_bit); 
          *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) |= PORT_B_Enable_bit;
        }
      break;
      case PORT_C_Enable_bit:
        if(!is_portC_configured){
          is_portC_configured = 1; //Only configure port RCC once 
          *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) &= ~(PORT_C_Enable_bit); 
          *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) |= PORT_C_Enable_bit;
        }
      break;
      case PORT_D_Enable_bit:
        if(!is_portD_configured){
          is_portD_configured = 1; //Only configure port RCC once 
          *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) &= ~(PORT_D_Enable_bit); 
          *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) |= PORT_D_Enable_bit;
        }
      break;
    }
    
    //Pin configuration
    CNF_MODE_bits_u8 = temp_pin_conf_str.Pin_Mode;
    if(CNF_MODE_bits_u8 == INPUT_MODE){
      CNF_MODE_bits_u8 |= (((uint8_t)temp_pin_conf_str.Pin_Input_cnf) << 2); //cnf_bits are 2 bit high, mode_bits are 2 bit low
    }
    else{
      CNF_MODE_bits_u8 |= (((uint8_t)temp_pin_conf_str.Pin_Output_cnf) << 2); //cnf_bits are 2 bit high, mode_bits are 2 bit low
    }
    
    //mapping to pin position in register
    CNF_MODE_bits_mapped_u32 = CR_reg_bit_mapping(temp_pin_conf_str.PIN_Index, CNF_MODE_bits_u8);
    pins_position_mapped_u32 = CR_reg_bit_mapping(temp_pin_conf_str.PIN_Index, 0x0f); //1111 would mask cnf_mode bits of the corresponding pin in the register

    if(temp_pin_conf_str.is_Reg_CRL){
      CR_reg_u32 = &(temp_pin_conf_str.GPIO_Port->CRL);
    }
    else{
      CR_reg_u32 = &(temp_pin_conf_str.GPIO_Port->CRH);
    }  

     
    *CR_reg_u32 &= ~(pins_position_mapped_u32); //clear current cnf_mode configuration of the pin
    *CR_reg_u32 |= CNF_MODE_bits_mapped_u32; //configure new cnf_mode bits
  }
}
