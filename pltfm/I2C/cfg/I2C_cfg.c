#include "I2C.h"
#include "GPIO.h"
#define CCR_BIT_MASK I2C_CCR_CCR
 
// @author: Huynh Khuong


#define I2C1_REGISTER     I2C1_EV_IRQn
#define I2C2_REGISTER     I2C2_EV_IRQn
#define I2C1_ERR_REGISTER I2C1_ER_IRQn
#define I2C2_ERR_REGISTER I2C2_ER_IRQn

#define I2C_NO_NVIC_REGISTER  (uint32_t)999 //dummy value representing NO NVIC REGISTER

#define I2C_SOFTRESET_BIT_MASK        I2C_CR1_SWRST
#define I2C_FREQ_BIT_MASK             I2C_CR2_FREQ
#define I2C_CCR_BIT_MASK              I2C_CCR_CCR
#define I2C_FS_BIT_MASK               I2C_CCR_FS
#define I2C_TRISE_TIME_BIT_MASK       I2C_TRISE_TRISE     
#define I2C_ADD_MODE_BIT_MASK         I2C_OAR1_ADDMODE
#define I2C_7BIT_ADD_BIT_MASK         I2C_OAR1_ADD1_7
#define I2C_10BIT_ADD_BIT_MASK        (I2C_OAR1_ADD1_7|I2C_OAR1_ADD8_9|I2C_OAR1_ADD0)

uint8_t is_I2C1_NVIC_registered = 0;
uint8_t is_I2C2_NVIC_registered = 0;

const I2C_cfg I2C_Conf_cst[NUM_OF_I2C_NODE] = {
  //I2C1 configure
  {
    //RCC
    {
      RCC_I2C1_EN,
      &(RCC->APB1ENR) 
    },
    I2C1,
    MASTER,
    //Address configuration
    {
      SEVEN_BIT_ADDR,
      0, //temporarily don't configure own address
      &(I2C1->OAR1)
    },
    SM_MODE,
    //Clock config
    {
      36, //36MHz
      0.1 //100KHz
    },
    //GPIO index to main GPIO configuration struct
    {
      4, //SDA
      5  //SCL
    }, 
    &is_I2C1_NVIC_registered,
    I2C1_REGISTER,
    I2C1_ERR_REGISTER
  }
};


/*
  Init procedure for master: 
  1. Program the I2C_CR2 register -> program the peripheral input clock 
  2. Configure the clock control register
  3. Configure the rise time register
  4. Configure the I2C_CR2 register -> Enable I2C
  5. Set the START_BIT To generate start condition

  Note that: the peripheral input clock must be at least: 
  - 2 MHz in standard speed
  - 4 MHz in fast speed
*/


/*
  @brief: Configure CCR params to control T_high and T_low in clock  
  @note: to configure clock for I2C, we have to take these 3 params into consideration
    1. CCR: T_high = T_low = CCR * T(input clock)
    2. FREQ: frequency of clock (This params must be configured to match with input clock)
    3. TRISE: this param relates to the sampling time of SCL pin when master generates clock
        if after sampling, SCL = 1: normal operation
        if after sampling, SCL = 0: the slave is stretching the line
*/

/*
  @brief: static function computing CCR value to write into CCR register
  @input: Frequency of SCL line
  @output: calculated value of CCR register
*/
static uint32_t CCR_clock_configuration(float SCL_freq_MHz_u32, uint32_t RCC_freq_MHz_u32){

  //fPLCK1 must be at least 2MHz to achieve Sm Mode
  //fPLCK1 must be at least 4MHz to achieve Fm Mode

  /*
    for S/m:
    T_high = T_low = TPLCK1 * CCR
    Currently, we just focus on S/M mde
  */

  uint32_t CCR_val_u32 = 0;
  float Period_clock_us = 1 / (float)SCL_freq_MHz_u32;  
  float T_high = Period_clock_us/2;

  CCR_val_u32 = ((uint32_t)T_high*RCC_freq_MHz_u32)& CCR_BIT_MASK;

  return CCR_val_u32;
}


/*
  @brief: static function computing maximum rise time register
  @input: CCR value 
  @output: calculated value of CCR register
*/
static uint32_t TRISE_TIME_Configuration(float SCL_freq_MHz_u32, uint32_t RCC_freq_MHz_u32){
  //Maximum Rise time in I2C sm is 1000ns
  //Maximum Rise time in I2c fm is 300ns
  //fm calculation would be handled later

  uint32_t TRISE_time_u32 = 0;
  float configured_rise_time_us = (1/SCL_freq_MHz_u32); 
  const float max_rise_time_sm_mode_us = 1;

  if(configured_rise_time_us < max_rise_time_sm_mode_us){
    TRISE_time_u32 = configured_rise_time_us;
  }
  else TRISE_time_u32 = max_rise_time_sm_mode_us;

  TRISE_time_u32 = (TRISE_time_u32*RCC_freq_MHz_u32) + 1;

  return TRISE_time_u32;
}

/*  
    @brief: Initiate I2C peripheral
    @param I2C_port_u8: index in main configuration struct
    @output: Configure address, check I/O pins and clock related params.
*/
void I2C_Init(void){
  
  I2C_cfg temp_I2C_conf_str;
  Address_mode I2C_Address_mode = SEVEN_BIT_ADDR;

  volatile uint32_t* AFIO_EN_reg = &(RCC->APB2ENR);
  
  uint8_t is_pin_check_passed = 1; //default value is true

  // For all other peripherals that uses I/O
  *AFIO_EN_reg &= ~0x01; //bit 0 disables the AFIO clock
  *AFIO_EN_reg |= 0x01; //bit 0 enables the AFIO clock

  //Iterate throughout all indexes of the struct
  for(uint8_t i = 0; i < NUM_OF_I2C_NODE; i++){

    temp_I2C_conf_str = I2C_Conf_cst[i];

    //Check is the GPIO pin configured or not? 
    for(uint8_t j = 0; j < TOTAL_PIN_PER_I2C; j ++){
      //Iterate throughout I2C mapped pins
      if(!is_pin_configured(temp_I2C_conf_str.pin[j])) {
        is_pin_check_passed = 0;
        break;
      }
    }

    if(!is_pin_check_passed) continue; //I/O pin check is not passed

    //configure for RCC
    *(temp_I2C_conf_str.I2C_RCC_config.Clock_cfg_reg) &= ~temp_I2C_conf_str.I2C_RCC_config.I2C_Node_enable; //clear current status
    *(temp_I2C_conf_str.I2C_RCC_config.Clock_cfg_reg) |= temp_I2C_conf_str.I2C_RCC_config.I2C_Node_enable;  //config new status

    //reset I2C 
    temp_I2C_conf_str.I2C_Node->CR1 |= I2C_SOFTRESET_BIT_MASK;
    temp_I2C_conf_str.I2C_Node->CR1 &= ~I2C_SOFTRESET_BIT_MASK;

    //CLock configuration
    //configure FREQ params
    temp_I2C_conf_str.I2C_Node->CR2|= ((uint32_t)temp_I2C_conf_str.SCL_desired_clock.RCC_Freq_Mhz & I2C_FREQ_BIT_MASK);

    //configure SM Mode or FM Mode
    temp_I2C_conf_str.I2C_Node->CCR &= I2C_FS_BIT_MASK;    
    temp_I2C_conf_str.I2C_Node->CCR |= temp_I2C_conf_str.I2C_Speed_Mode;    

    //configure DUTY in the future (applicable for FM mode only)

    //configure CCR value (T)
    temp_I2C_conf_str.I2C_Node->CCR |= (CCR_clock_configuration(temp_I2C_conf_str.SCL_desired_clock.SCL_clock_Mhz, temp_I2C_conf_str.SCL_desired_clock.RCC_Freq_Mhz) & I2C_CCR_BIT_MASK );

    //configure Maximum rise time
    temp_I2C_conf_str.I2C_Node->TRISE &= ~(I2C_TRISE_TIME_BIT_MASK);
    temp_I2C_conf_str.I2C_Node->TRISE |= (TRISE_TIME_Configuration(temp_I2C_conf_str.SCL_desired_clock.SCL_clock_Mhz, temp_I2C_conf_str.SCL_desired_clock.RCC_Freq_Mhz) & I2C_TRISE_TIME_BIT_MASK);

    //Address configuration
    //configure address mode: 7BIT or 10BIT
    *(temp_I2C_conf_str.I2C_Node_adress.Address_config_reg) &= ~I2C_ADD_MODE_BIT_MASK;         
    *(temp_I2C_conf_str.I2C_Node_adress.Address_config_reg) |= (temp_I2C_conf_str.I2C_Node_adress.Address_length);
    I2C_Address_mode  = temp_I2C_conf_str.I2C_Node_adress.Address_length;

    //configure device address: 
    *(temp_I2C_conf_str.I2C_Node_adress.Address_config_reg) &= ~I2C_10BIT_ADD_BIT_MASK;         

    //configure differently based on 7BIT or 10BIT
    if(I2C_Address_mode == SEVEN_BIT_ADDR){
      *(temp_I2C_conf_str.I2C_Node_adress.Address_config_reg) |= (temp_I2C_conf_str.I2C_Node_adress.Address_u16 << 1) & I2C_7BIT_ADD_BIT_MASK;
    }
    else { //10bit mode is only configured in advanced, extended address handling is not taken into consideration in current context
      *(temp_I2C_conf_str.I2C_Node_adress.Address_config_reg) |= (temp_I2C_conf_str.I2C_Node_adress.Address_u16) & I2C_10BIT_ADD_BIT_MASK;
    }

    //NVIC registeration for I2C Node
    if(*(temp_I2C_conf_str.is_I2C_node_NVIC_registered) == 0){ //UART module is not registered in NVIC
      //There are 2 indexes in NVIC for I2C 
      if(temp_I2C_conf_str.NVIC_index_1 < I2C_NO_NVIC_REGISTER){
        __set_PRIMASK(1);
        NVIC_EnableIRQ(temp_I2C_conf_str.NVIC_index_1);
        __set_PRIMASK(0);
      }

      if(temp_I2C_conf_str.NVIC_index_2 < I2C_NO_NVIC_REGISTER){
        __set_PRIMASK(1);
        NVIC_EnableIRQ(temp_I2C_conf_str.NVIC_index_2);
        __set_PRIMASK(0);
      }
      *(temp_I2C_conf_str.is_I2C_node_NVIC_registered) = 1;
    }
  }
}
