#include "UART.h"
#include "GPIO.h"


#define NO_PIN_CONFIGURED   (uint32_t)33
#define UE_BIT_MASK         USART_CR1_UE

#define M_BIT_POS           (uint32_t)12
#define M_BIT_MASK          USART_CR1_M

#define STOP_BITS_MASK      USART_CR2_STOP     
#define STOP_BITS_POS       (uint32_t)12


#define USART1_REGISTER     USART1_IRQ
#define USART2_REGISTER     USART2_IRQ
#define USART3_REGISTER     USART3_IRQ

uint32_t is_UART1_NVIC_registered = 0;
uint32_t is_UART2_NVIC_registered = 0;
uint32_t is_UART3_NVIC_registered = 0;

//Main struct containing all UARTS configuration
const UART_cfg UART_conf_cst[NUMB_OF_UART_CONFIGURE] = {
  {
    //Base configuration
    USART1, //UART NODE
    {
      RCC_UART1_EN,   //RCC enable bit mapped on reg_u32
      &RCC->APB2ENR
    },   //RCC section
    {
      2,
      NO_PIN_CONFIGURED
    },   //GPIO pin index in the GPIO_conf_struct

    //Communication config
    TX,   //communcation side
    EIGHT_BITS_FRAME, //word length
    ONE_STOP_BIT,   //numb_of_stop_bits
    {
      115200,
      &USART1->BRR
    },
    &is_UART1_NVIC_registered,
    USART1_IRQ
  }
};

/*


  @brief: Compute the Mantissa part and Fraction part to write in to the BR configuration register 
  @input: desired baudrate_value
  @output: uint32_t register containing DIV_Mantissa part & DIV_Fraction part
*/
static uint32_t Baudrate_calc(uint32_t baudrate_val_u32, uint8_t UART_index){
  uint32_t DIV_Mantissa_u32 = 0;
  uint32_t DIV_Fraction_u32 = 0;
  uint32_t BRR_u32_register = 0 ;
  float DIV_value_f = 0;
  //SYSTEM uses 1st RCC profile in RCC module
  //APB2 clock is 72 MHz
  //APB1 clock is 36 MHz
  //Calculation would be based on each module, 
  const UART_cfg current_UART_index = UART_conf_cst[UART_index];

  if(current_UART_index.UART_node == USART1){
    //Then uses APB2 clock for computation
    DIV_value_f = ((float)72000000)/baudrate_val_u32/16; 
  }
  else {
    //Then uses APB1 clock for computation
    DIV_value_f = ((float)36000000)/baudrate_val_u32/16; 
  }

  DIV_Mantissa_u32 = (uint32_t)(DIV_value_f);
  DIV_Fraction_u32 = (uint32_t)((DIV_value_f - DIV_Mantissa_u32)*16);

  //Construct uint32_t array
  BRR_u32_register = DIV_Mantissa_u32 << 4;
  BRR_u32_register |= (DIV_Fraction_u32 & 0x0F);

  return BRR_u32_register;
}

/*
  @brief: Init all UART modules registered in the main configured struct
  @input: Void, the function would take the input from the global const struct to configure UART
          The members of struct are in a TX/RX level of the UART
  @output: Void
*/
void UART_Init(void){
  /*
    /-------------- Configuring procedure --------------------/
    1. Enable UART: UE bit
    2. M bit to define the word length
    3. Program the number of stop bit 
    4. DMA enable (optional)
    5. Select the desired baudrate
  */

  UART_cfg temp_UART_conf_str;
  volatile uint32_t* AFIO_EN_reg = &(RCC->APB2ENR);
  
  uint8_t is_pin_check_passed = 1; //default value is true

  // For all other peripherals that uses I/O
  *AFIO_EN_reg &= ~0x01; //bit 0 disables the AFIO clock
  *AFIO_EN_reg |= 0x01; //bit 0 enables the AFIO clock

  //Iterate throughout all indexes of the struct
  for(uint8_t i = 0; i < NUMB_OF_UART_CONFIGURE; i++){
    temp_UART_conf_str = UART_conf_cst[i];

    //Check is the GPIO pin configured or not? 
    for(uint8_t j = 0; j < TOTAL_PIN_PER_UART; j ++){
      //Iterate throughout UART mapped pins
      if(temp_UART_conf_str.pin[j] == NO_PIN_CONFIGURED) continue;
      if(!is_pin_configured(temp_UART_conf_str.pin[j])) {
        is_pin_check_passed = 0;
        break;
      }
    }

    if(!is_pin_check_passed) continue; //Skip, don't continue configuring the current UART module

    //Configure for RCC
    *(temp_UART_conf_str.UART_RCC_config.Clock_config_reg) &= ~temp_UART_conf_str.UART_RCC_config.UART_node_EN; //clear current status
    *(temp_UART_conf_str.UART_RCC_config.Clock_config_reg) |= temp_UART_conf_str.UART_RCC_config.UART_node_EN; //config new status

    //Enable UART
    temp_UART_conf_str.UART_node->CR1 |= UE_BIT_MASK;          

    //M bit
    temp_UART_conf_str.UART_node->CR1 &= ~M_BIT_MASK;  //clear current status
    temp_UART_conf_str.UART_node->CR1 |= ((uint32_t)temp_UART_conf_str.FRAME_LENGTH << M_BIT_POS); //config new status

    //numb of stop bit
    temp_UART_conf_str.UART_node->CR2 &= ~STOP_BITS_MASK;  //clear current status
    temp_UART_conf_str.UART_node->CR2 |= ((uint32_t)temp_UART_conf_str.FINISHER_BIT << STOP_BITS_POS); //config new status

    //Select baudrate
    *temp_UART_conf_str.Baudrate.Baudrate_register = Baudrate_calc(temp_UART_conf_str.Baudrate.Baud_rate_val_u32, i);

    //Interrupt request register for UART
    if(*(temp_UART_conf_str.is_NODE_registered) == 0){ //UART module is not registered in NVIC
      __set_PRIMASK(1);
      NVIC_EnableIRQ(temp_UART_conf_str.NVIC_index);
      __set_PRIMASK(0);
      *(temp_UART_conf_str.is_NODE_registered) = 1;
    }
  } 
}
