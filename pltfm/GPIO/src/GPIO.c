#include "GPIO.h"

static uint32_t BSRR_Remapping(uint32_t PIN, uint32_t bit_state){
/*
  @brief: an internal functions helping to re-map pin index to its position in BSRR register
*/
  //BSRR register is 32-bit long, devided into 2 sections
  //16 high bits are used to reset state of 16 pins 
  //16 low bits are used to set state of 16 pins 
  uint32_t mapped_pos_u32 = 0;

  if(bit_state){ //set state 
    mapped_pos_u32 |= ((uint32_t)1 << PIN);
  }
  else{ //reset state
    mapped_pos_u32 |= ((uint32_t)1 << (PIN+16)); //reset pin 0 starts from bit 16
  } 

  return mapped_pos_u32;
}

/*
  @brief: Write value to a specific GPO pin
  @input: pin_index: the index of pin in the GPIO main configuration struct, state: on(1)/off(0)
  @output: void
*/
void GPIO_WritePin(uint32_t pin_index, uint32_t state){ 
  //error check
  if(pin_index > NUM_OF_PINS_CONFIGURE) return;  
  const GPIO_conf temp_GPIO_conf = GPIO_conf_cst[pin_index];

  //Check If the pin is output or input
  if(temp_GPIO_conf.Pin_Mode == INPUT_MODE) return; 

  // //manipulate output via BSRR register 
  temp_GPIO_conf.GPIO_Port->BSRR = BSRR_Remapping(temp_GPIO_conf.PIN_Index, state);
  return;
}

/*
  @brief: Read value from a specific GPI pin
  @input: pin_index: the index of pin in the GPIO main configuration struct
  @output: on(1), off(0)
*/
uint32_t GPIO_ReadPin(uint32_t pin_index){ 
  //error check
  if(pin_index > NUM_OF_PINS_CONFIGURE) return 0;  

  const GPIO_conf temp_GPIO_conf = GPIO_conf_cst[pin_index];
  uint32_t DR_reg_value_u32 = 0;
  const uint32_t pin_mapping_pos_u32 = ((uint32_t)1 << temp_GPIO_conf.PIN_Index); //get pin position in IDR register corresponding to PIN index

  //Check If the pin is output or input
  if(temp_GPIO_conf.Pin_Mode > INPUT_MODE) return 0; 

  //Read input state via DR register
  DR_reg_value_u32 = temp_GPIO_conf.GPIO_Port->IDR;

  //Get input value of the pin only
  DR_reg_value_u32 &= pin_mapping_pos_u32;
  DR_reg_value_u32 >>= temp_GPIO_conf.PIN_Index;

  return DR_reg_value_u32; 
}
