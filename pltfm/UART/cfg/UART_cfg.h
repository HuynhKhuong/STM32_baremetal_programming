#ifndef __UART_CFG_H__
#define __UART_CFG_H__
#include "stm32f10x.h"

/************************
@Author: Huynh Khuong
Define structures to add configurations to UART peripherals
*************************/

/*
  STM32's UART Characteristic:
  - Support 8, 9 bit length
  - Idle character is 1
  - break character is 0 
  - Transmission & Reception are driven by a common baudrate generator 
  - The clock for each side is generated when the enable bit is set respectively for the transmitter and receiver.
*/

/*
  Transmission Characteristic: 
  - Data shifts out LSB first (on the Tx Pin)
  - UART_DR consists of a buffer (TDR) between the internal bus and transmit shift register 
  - Every character is perceived with a start bit, The character is terminated with a number of stop bits 

  - TE bit should not be reset during the transmission of data. 
*/

//Flags indicating whether the UART Interrupt is registered or not 
extern uint32_t is_UART1_NVIC_registered;
extern uint32_t is_UART2_NVIC_registered;
extern uint32_t is_UART3_NVIC_registered;

#define NUMB_OF_UART_CONFIGURE      (uint8_t)1
#define TOTAL_PIN_PER_UART             (uint8_t)2      

//member enum indicating TX or RX side
typedef enum{
  TX = (uint8_t)0, 
  RX = (uint8_t)1
}communication_side;

//member enum indicating word length (the length of a character would be transmitted/received) 
typedef enum{
  EIGHT_BITS_FRAME = (uint8_t)0,
  NINE_BITS_FRAME = (uint8_t)1
}word_length;

//member enum indicating number of stop bit (after the number of stop bits, MCU would transmit/receive new character)
typedef enum{
  ONE_STOP_BIT = (uint8_t)0,
  HALF_STOP_BIT = (uint8_t)1,
  TWO_STOP_BIT = (uint8_t)2, 
  ONE_HALF_STOP_BIT = (uint8_t)3
}num_stop_bit;               

//member struct configuring DMA (this would be the future feature)

//member enums configuring RCC (clock enable)
typedef enum{
  RCC_UART1_EN = (uint32_t)RCC_APB2ENR_USART1EN,
  RCC_UART2_EN = (uint32_t)RCC_APB1ENR_USART2EN,
  RCC_UART3_EN = (uint32_t)RCC_APB1ENR_USART3EN//Currently we use maximum 3 UART nodes only
}RCC_enable_UART;

//member struct configuring clock sync
typedef struct{
  RCC_enable_UART UART_node_EN;
  volatile uint32_t* Clock_config_reg; //register configuring RCC
}Clock_config;


//member struct configuring Baudrate
typedef struct{
  uint32_t Baud_rate_val_u32;
  volatile uint16_t* Baudrate_register; //Baudrate register
}Baudrate_configure;

/*
  GPIO configuration design
  - How to not interfere with GPIO module? 
  - GPIO module has already registered pins for UART (in )
  - UART just has the index of the pin configuration struct array 
  - UART just checks if the GPIO module has configured the pin or not: GPIO provides other components method to check 
  GPIO configuration
  - If not then stop don't configure any UART module 
*/


//Member struct requesting Interrupt
typedef struct{
  // 2 requests containing 2 enable bits
  uint32_t request1_u32;
  uint32_t request2_u32;
}EX_request;

//main struct containing all UART configuration
typedef struct{
  //Base configuration
  USART_TypeDef* UART_node;      //UART NODE
  Clock_config UART_RCC_config; //RCC section
  uint8_t pin[TOTAL_PIN_PER_UART]; //GPIO config

  //Communication config
  communication_side Comm_DIR;
  word_length FRAME_LENGTH;
  num_stop_bit  FINISHER_BIT;
  Baudrate_configure Baudrate;
  EX_request Interrupt_enquire;

  //NVIC register
  uint32_t* is_NODE_registered;
  uint32_t NVIC_index;
}UART_cfg;

/*-------------------------------------------------------------------------*/
extern const UART_cfg UART_conf_cst[NUMB_OF_UART_CONFIGURE];

#endif 
