#include "GPIO.h"

///  @brief: an internal functions helping to re-map pin index to its position in BSRR register
uint32_t BSRR_Remapping(uint32_t PIN, uint32_t bit_state){
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

/*
  @brief: Check register value and compare with the index, anounce user whether the pin is configured correctly
  @input: pin_index: the index of pin in the GPIO main configuration struct
  @output: correctly(1), incorrectly(0)
*/

uint32_t is_pin_configured(uint32_t pin_index){
  
  uint32_t result = 1; //default value is true

  //Extract information from struct
  const GPIO_conf pin_conf_cst = GPIO_conf_cst[pin_index];
  const GPIO_MODE pin_MODE = pin_conf_cst.Pin_Mode;
  const uint8_t pin_CNF = (pin_MODE == INPUT_MODE)?pin_conf_cst.Pin_Input_cnf:pin_conf_cst.Pin_Output_cnf; 
  volatile uint32_t* conf_reg = (pin_conf_cst.is_Reg_CRL)?&(pin_conf_cst.GPIO_Port->CRL):&(pin_conf_cst.GPIO_Port->CRH);

  //Extract information from hardware register
  const uint32_t pin_configured_status = Get_pin_configure_status(pin_conf_cst.PIN_Index, 0x0f, conf_reg);
  const uint32_t MODE_value_u32 = pin_configured_status & 0x03; // 0x0011b
  const uint32_t CNF_value_u32 = pin_configured_status >> 2; //0x1100b

  //Check port configuration: RCC
  if(pin_conf_cst.is_port_configured == 0){
    result = 0;
    return result; 
  }

  //Check pin configuration: MODE
  if(pin_MODE != MODE_value_u32){
    result = 0;
    return result;
  }

  //Check pin configuration: CNF
  if(pin_CNF != CNF_value_u32){
    result = 0;
    return result;
  }

  //All conditions passed
  return result;
}

/// @brief this API provides user the ability to dynamically disable EXT_Interrupt on a specific line
/// @param pin_index: pin_index in the main configure struct
void GPIO_EXT_disable(uint32_t pin_index){
  //To disable Interrupt line
  //Disable Interrupt Mask and NVIC_registeration
  GPIO_conf temp_pin_conf_str = GPIO_conf_cst[pin_index];

  EXTI->IMR &= ~(((uint8_t)0x01) << temp_pin_conf_str.PIN_Index);
  DISABLE_INTERRUPT(temp_pin_conf_str.Pin_Interrupt_cnf.NVIC_index);
}
