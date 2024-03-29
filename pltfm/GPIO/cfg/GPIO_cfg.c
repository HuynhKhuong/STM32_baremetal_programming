#include "GPIO.h"
#include <stdio.h>

/************************
@Author: Huynh Khuong
Configure structures to add configurations to GPIO peripherals
Configuring logic to init GPIO peripherals 
*************************/

#define EXTI_Line0_Interrupt EXTI0_IRQn
#define EXTI_Line1_Interrupt EXTI1_IRQn
#define EXTI_Line2_Interrupt EXTI2_IRQn
#define EXTI_Line3_Interrupt EXTI3_IRQn
#define EXTI_Line4_Interrupt EXTI4_IRQn

//flags indicating port status
uint8_t is_portA_configured = 0;
uint8_t is_portB_configured = 0;
uint8_t is_portC_configured = 0;
uint8_t is_portD_configured = 0;

//flags indicating interrupt line status
uint8_t is_line_0_configured = 0;
uint8_t is_line_1_configured = 0;
uint8_t is_line_2_configured = 0;
uint8_t is_line_3_configured = 0;
uint8_t is_line_4_configured = 0;
uint8_t is_line_5_configured = 0;
uint8_t is_line_6_configured = 0;
uint8_t is_line_7_configured = 0;
uint8_t is_line_8_configured = 0;
uint8_t is_line_9_configured = 0;
uint8_t is_line_10_configured = 0;
uint8_t is_line_11_configured = 0;
uint8_t is_line_12_configured = 0;
uint8_t is_line_13_configured = 0;
uint8_t is_line_14_configured = 0;
uint8_t is_line_15_configured = 0;

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
  OUTPUT_PP,
  {
    NO_EVENT,
    NULL, //This variable would be discarded
    NULL,
    PORT_C_pin_x,
    0, //This variable would be discarded
  }
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
  PD_INPUT,
  NOT_OUTPUT,
  {
    NO_EVENT,
    NULL, //This variable would be discarded
    NULL,
    PORT_A_pin_x,
    0, //This variable would be discarded
  }
},

//3. PIN 9A, UART TX PIN, ALT_OUTPUT
{
  {
    &(RCC->APB2ENR), //RCC reg
    PORT_A_Enable_bit
  },
  GPIOA,
  &is_portA_configured,
  PIN_9,
  0, //reg CRH not CRL
  OUTPUT_MODE_50MHz,
  NOT_INPUT,
  AL_OUTPUT_PP,
  {
    NO_EVENT,
    NULL, //This variable would be discarded
    NULL,
    PORT_A_pin_x,
    0, //This variable would be discarded
  }
},

//4. PIN 10A, UART RX PIN, FLOATING INPUT
{
  {
    &(RCC->APB2ENR), //RCC reg
    PORT_A_Enable_bit
  },
  GPIOA,
  &is_portA_configured,
  PIN_10,
  0, //reg CRH not CRL
  INPUT_MODE,
	FLOATING_INPUT,
	NOT_OUTPUT,
  {
    NO_EVENT,
    NULL, //This variable would be discarded
    NULL,
    PORT_A_pin_x,
    0, //This variable would be discarded
  }
},

//5. PIN 6B, I2C SCL, AF OPEN_DRAIN
{
  {
    &(RCC->APB2ENR), //RCC reg
    PORT_B_Enable_bit
  },
  GPIOB,
  &is_portB_configured,
  PIN_6,
  1, //reg CRL not CRH
  OUTPUT_MODE_50MHz,
  NOT_INPUT,
  AL_OUTPUT_OD,
  {
    NO_EVENT,
    NULL, //This variable would be discarded
    NULL,
    PORT_B_pin_x,
    0, //This variable would be discarded
  }
},

//6. PIN 7B, I2C SDA, AF OPEN_DRAIN 
{
  {
    &(RCC->APB2ENR), //RCC reg
    PORT_B_Enable_bit
  },
  GPIOB,
  &is_portB_configured,
  PIN_7,
  1, //reg CRL not CRH
  OUTPUT_MODE_50MHz,
  NOT_INPUT,
  AL_OUTPUT_OD,
  {
    NO_EVENT,
    NULL, //This variable would be discarded
    NULL,
    PORT_B_pin_x,
    0, //This variable would be discarded
  }
},

//7. PIN 0A, INPUT PULL-UP, PULL-DOWN, INTERRUPT LINE 0
{
	{
    &(RCC->APB2ENR), //RCC reg
    PORT_A_Enable_bit
  },
  GPIOA,
  &is_portA_configured,
  PIN_0,
  1, //reg CRL not CRH
  INPUT_MODE,
  PU_INPUT,
  NOT_OUTPUT,
  {
    BOTH_RISING_FALLING_EDGE,
    &is_line_0_configured,
    &AFIO->EXTICR[0],
    PORT_A_pin_x,
    EXTI_Line0_Interrupt
  }
},
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


///  @brief: A kernel mapping bits to CRH/CRL GPIO register
///  @input: PIN index (0->31), BITS value (ranges from 0-15, depends on what PIN index)
///  @output: values which are mapped to the pin position in the AF register

static uint32_t AF_reg_bit_mapping(uint32_t PIN, uint32_t BITS) {
  //STM32 AF registers are devided into 4 regs, each supports 4 pins configuration 
  //In Author's definition, pins are described from 0 -> 31
  //Their index should be remapped into 4-pin groups before being mapped into AF registers
  uint32_t pin_group_mapping_u32 = (PIN%4);
  return (uint32_t)(((uint32_t)0 | BITS) << (pin_group_mapping_u32*BIT_POS_SHIFT));
}

/// @brief static function configuring the EXT line if required
/// @param pin_index: the index of the pin in the main configuring struct

static void EXTI_Configure(uint8_t pin_index){

  GPIO_conf temp_pin_conf_str = GPIO_conf_cst[pin_index];
  uint8_t rising_edge_configure_flag = (temp_pin_conf_str.Pin_Interrupt_cnf.interrupt_trigger_event == BOTH_RISING_FALLING_EDGE) || (temp_pin_conf_str.Pin_Interrupt_cnf.interrupt_trigger_event == RISING_EDGE);
  uint8_t falling_edge_configure_flag = (temp_pin_conf_str.Pin_Interrupt_cnf.interrupt_trigger_event == BOTH_RISING_FALLING_EDGE) || (temp_pin_conf_str.Pin_Interrupt_cnf.interrupt_trigger_event == FALLING_EDGE);

  //Pre-condition check
  if(temp_pin_conf_str.Pin_Interrupt_cnf.interrupt_trigger_event == NO_EVENT) return; //This pin is not meant interrupt configured
  if(temp_pin_conf_str.Pin_Mode > INPUT_MODE) return;                                 //To configure EXT interrupt, the pin must be input mode
  if(*(temp_pin_conf_str.Pin_Interrupt_cnf.is_interrupt_line_configured) == 1) return;//This EXT line has been configured

  //Configure EXT mask pin
  EXTI->IMR |= ((uint8_t)0x01) << temp_pin_conf_str.PIN_Index;

  //Configure trigger event type
  if(rising_edge_configure_flag == 1){
    EXTI->RTSR |= ((uint8_t)0x01) << temp_pin_conf_str.PIN_Index;
  }

  if(falling_edge_configure_flag ){
    EXTI->FTSR |= ((uint8_t)0x01) << temp_pin_conf_str.PIN_Index;
  }

  //Configure pin as INPUT AF Pin
  *(temp_pin_conf_str.Pin_Interrupt_cnf.AF_Register) |= AF_reg_bit_mapping(temp_pin_conf_str.PIN_Index,temp_pin_conf_str.Pin_Interrupt_cnf.Port);

  //Enable interrupt with NVIC
  ENABLE_INTERRUPT(temp_pin_conf_str.Pin_Interrupt_cnf.NVIC_index);
	
	//Mark that this interrupt line has been configured
	*(temp_pin_conf_str.Pin_Interrupt_cnf.is_interrupt_line_configured) = 1;
}

/*
  @brief: a mini API, wraps up CR_reg_bit_mapping function, to get the CNF/MODE status of a specific pin in CRH/CRL register
  @input: PIN index (0->31), BITS value (ranges from 0-15, depends on what CNF and MODE values are configured for that pin)
          reg: CRH or CRL
  @output: values which are mapped to the pin position in that register
*/
uint32_t Get_pin_configure_status(uint32_t PIN, uint32_t BITS, volatile uint32_t* reg){

  uint32_t pin_pos_u32 = CR_reg_bit_mapping(PIN, BITS);
  uint32_t pin_status_u32 = (*reg) & pin_pos_u32;

  pin_status_u32 >>= ((PIN%8)*BIT_POS_SHIFT);

  return pin_status_u32;
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

  volatile uint32_t* AFIO_EN_reg = &(RCC->APB2ENR);
  
  // For all other peripherals that uses I/O
  *AFIO_EN_reg &= ~0x01; //bit 0 disables the AFIO clock
  *AFIO_EN_reg |= 0x01; //bit 0 enables the AFIO clock
  
  //Iterate through all pin configuration
  for(uint8_t i = 0; i < NUM_OF_PINS_CONFIGURE; i++){
    temp_pin_conf_str = GPIO_conf_cst[i];

    //check pre-condition to configure port RCC
		if(*(temp_pin_conf_str.is_port_configured) == 0){
			*(temp_pin_conf_str.is_port_configured) = 1; //Only configure port RCC once 
      *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) &= ~(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_bit_u32); 
      *(temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_reg_u32) |= temp_pin_conf_str.GPIO_Port_RCC_cfg_str.Conf_bit_u32;
		}
    
    //Pin configuration
    CNF_MODE_bits_u8 = temp_pin_conf_str.Pin_Mode;
    if(CNF_MODE_bits_u8 == INPUT_MODE){
      if(temp_pin_conf_str.Pin_Input_cnf == PD_INPUT){
        ///This logic is specific for PD_Input, 
        ///   as PD_Input, PU_Input would have the same configuration value in CNF register
        ///   However, the value of PD_Input - 1 =  PU_Input (which is the right configuration value in CNF_register)
        CNF_MODE_bits_u8 |= (((uint8_t)temp_pin_conf_str.Pin_Input_cnf - 1) << 2); //cnf_bits are 2 bit high, mode_bits are 2 bit low
      }
      else{
        CNF_MODE_bits_u8 |= (((uint8_t)temp_pin_conf_str.Pin_Input_cnf) << 2); //cnf_bits are 2 bit high, mode_bits are 2 bit low
      }
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
		
    //This is specific for input pin: i
    // - if the pin is configured as pull-up, then set its bit in ODR register to 1
    // - if the pin is configured as pull-down, then set its bit in ODR register to 0  
    if(temp_pin_conf_str.Pin_Mode == INPUT_MODE){
      if(temp_pin_conf_str.Pin_Input_cnf == PU_INPUT){//manipulate output via BSRR register 
				temp_pin_conf_str.GPIO_Port->BSRR = BSRR_Remapping(temp_pin_conf_str.PIN_Index, 1);
      }
      else if(temp_pin_conf_str.Pin_Input_cnf == PD_INPUT){
        //By default the pin is configured as PD_INPUT (If the configuration in CNF register is Pull-up Pull-down input)
        //Do nothing
      }
    }

		EXTI_Configure(i); //Configure EXT interrupt for corresponding pin
  }
}
