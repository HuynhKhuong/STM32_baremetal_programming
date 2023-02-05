#include "UART.h"

#define USART1_REGISTER  USART1_IRQ
#define USART2_REGISTER  USART2_IRQ
#define USART3_REGISTER  USART3_IRQ

uint32_t is_UART1_NVIC_registered = 0;
uint32_t is_UART2_NVIC_registered = 0;
uint32_t is_UART3_NVIC_registered = 0;

//Main struct containing all UARTS configuration
const UART_cfg UART_conf_cst[NUMB_OF_UART_CONFIGURE] = {
  {
    //Base configuration
    USART1, //UART NODE
    {
      RCC_UART1_EN,
      &RCC->APB2ENR
    },   //RCC section
    {
      2,
      3
    },   //GPIO pin index in the GPIO_conf_struct

    //Communication config
    TX,   //communcation side
    EIGHT_BITS_FRAME, //word length
    ONE_STOP_BIT,   //numb_of_stop_bits
    {
      115200,
      &USART1->BRR
    },             //baudrate configure
    {
      USART_CR1_TXEIE,
      USART_CR1_TCIE
    },              //Interrupt request
    &is_UART1_NVIC_registered,
    USART1_IRQ
  }
};

/*
  Procedure of Transmitting a character
  1. Enable UART: UE bit
  2. M bit to define the word length
  3. Program the number of stop bit 
  4. DMA enable (optional)
  5. Select the desired baudrate
  /-------------- Configuring phase --------------------/
  6. Set TE to send an IDLE frame and first transmission 
  7. Write Data to send in the DR register (this clears the TXE bit)
  8. After writing the last data into the UART_DR register, wait until TC= 1. This indicates that 
  the transmission of the last frame is complete
  9. Repeat the this for each data in case of single buffer 
*/

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
