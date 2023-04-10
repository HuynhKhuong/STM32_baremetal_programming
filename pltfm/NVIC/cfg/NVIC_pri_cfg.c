#include "NVIC_pri_cfg.h"


const pri_group_div global_pri_group = PRIGROUP_TWO_TWO;

Interrupt_prio_setup  interrupt_priority_conf[NUM_OF_INTERRUPTS_REGISTERED] = {
  ////Group1
  {
    USART1_IRQ, //UART1 interrupt
    0,
    0
  },
  {
    I2C1_EV_IRQn,///I2C1 Event interrupt
    0,
    1
  },
  {
    I2C1_ER_IRQn, ///I2C1 Error interrupt
    0,
    2
  }
};

/// @brief configure priority group structure: number of bits for pre-empt priority and sub priority
/// @param void: would use global_pri_group variable to configur the structure of priority group 
/// @return
static void Pri_group_setup(void){
  SET_PRIORITYGROUP(global_pri_group);
}

/// @brief main function would  
/// @param void: would use main configuration struct to config 
/// @return 
void Excp_pri_cfg(void){
  uint8_t is_interrupt_registered = 1;
  uint32_t final_priority_val = 0;
	Interrupt_prio_setup temp_interrupt_prio_setup;

  const uint32_t sub_prio_index = (global_pri_group);
  const uint32_t max_pre_emption_value = ((MAX_NUMB_PRE_EMPTION_BIT - global_pri_group) == 0)? 0:((1 << (MAX_NUMB_PRE_EMPTION_BIT - global_pri_group)) - 1);
  const uint32_t max_sub_prio_value = (global_pri_group == 0)?0:((1 << global_pri_group) - 1);
	
  //Setup priority group first
  Pri_group_setup();

  // Iterate through all registered interrupt
  for(uint8_t i = 0; i < NUM_OF_INTERRUPTS_REGISTERED; i++){
		temp_interrupt_prio_setup = interrupt_priority_conf[i];
    is_interrupt_registered  = GET_INTERRUPT_REGISTERED_STATUS(temp_interrupt_prio_setup.interrupt_instance);

    
    if(temp_interrupt_prio_setup.pre_emption > max_pre_emption_value) continue;
    if(temp_interrupt_prio_setup.sub_priority > max_sub_prio_value) continue;

    final_priority_val  = (temp_interrupt_prio_setup.pre_emption << sub_prio_index) | temp_interrupt_prio_setup.sub_priority;
		if(is_interrupt_registered == 0){
			SET_INTERRUPT_REGISTERED_PRIORITY(temp_interrupt_prio_setup.interrupt_instance, final_priority_val);		
		}
		else{
			DISABLE_INTERRUPT(temp_interrupt_prio_setup.interrupt_instance);     
			SET_INTERRUPT_REGISTERED_PRIORITY(temp_interrupt_prio_setup.interrupt_instance, final_priority_val);		
			ENABLE_INTERRUPT(temp_interrupt_prio_setup.interrupt_instance);      
		}
  }
}
