#include "NVIC_pri.h"

/******************************
 @author Huynh Khuong

 @brief Public apis for user to interact with exception priority order

********************************/

#define DISABLE_INTERRUPT(interrupt_instance)     __NVIC_DisableIRQ(interrupt_instance)
#define ENABLE_INTERRUPT(interrupt_instance)      __NVIC_EnableIRQ(interrupt_instance)
#define INTERRUPT_NOT_REGISTERED_FLAG             (uint32_t)16

/// @brief Update interrupt priority order of a specific interrupt, Update the setup in the main configuration struct
/// @param interrupt_index index of an interrupt instance in main configuration struct
/// @param pre_emption_val_u32 MAX = 15, MIN = 0
/// @param sub_priority_val_u32 MIN = 0, MAX = 15

void Set_Interrupt_prio(uint8_t interrupt_index, uint32_t pre_emption_val_u32, uint32_t sub_priority_val_u32){

  uint8_t is_interrupt_registered = 1;
  uint32_t final_priority_val = 0;

  const uint32_t sub_prio_index = (global_pri_group);
  const uint32_t max_pre_emption_value = ((MAX_NUMB_PRE_EMPTION_BIT - global_pri_group) == 0)? 0:((1 << (MAX_NUMB_PRE_EMPTION_BIT - global_pri_group)) - 1);
  const uint32_t max_sub_prio_value = (global_pri_group == 0)?0:((1 << global_pri_group) - 1);

  if(interrupt_index >= NUM_OF_INTERRUPTS_REGISTERED) return; ///OUT OF CONFIGUARTION STRUCT

	Interrupt_prio_setup *temp_interrupt_prio_setup = &interrupt_priority_conf[interrupt_index];

	
  is_interrupt_registered  = GET_INTERRUPT_REGISTERED_STATUS(temp_interrupt_prio_setup->interrupt_instance);

  if(is_interrupt_registered == 0) return;
  if(temp_interrupt_prio_setup->pre_emption > max_pre_emption_value) return;
  if(temp_interrupt_prio_setup->sub_priority > max_sub_prio_value) return;

  final_priority_val  = (temp_interrupt_prio_setup->pre_emption << sub_prio_index) | temp_interrupt_prio_setup->sub_priority;

  ///setup priority and modify main configuration struct
  temp_interrupt_prio_setup->pre_emption = pre_emption_val_u32;
  temp_interrupt_prio_setup->sub_priority = sub_priority_val_u32;
  

  DISABLE_INTERRUPT(temp_interrupt_prio_setup->interrupt_instance);     
  SET_INTERRUPT_REGISTERED_PRIORITY(temp_interrupt_prio_setup->interrupt_instance, final_priority_val);
  ENABLE_INTERRUPT(temp_interrupt_prio_setup->interrupt_instance);      
}


/// @brief Get interrupt priority order of a specific interrupt, Update the setup in the main configuration struct
/// @param interrupt_index index of an interrupt instance in main configuration struct
/// @return the encoded register value of that interrupt instance

uint32_t Get_Interrupt_prio(uint8_t interrupt_index){

  uint8_t is_interrupt_registered = 1;
  uint32_t final_priority_val = 0;

  const uint32_t sub_prio_index = (global_pri_group);
	Interrupt_prio_setup *temp_interrupt_prio_setup = &interrupt_priority_conf[interrupt_index];
	
  is_interrupt_registered  = GET_INTERRUPT_REGISTERED_STATUS(temp_interrupt_prio_setup->interrupt_instance);

  if(is_interrupt_registered == 0){
    final_priority_val = INTERRUPT_NOT_REGISTERED_FLAG;
  }
  else{
    final_priority_val  = (temp_interrupt_prio_setup->pre_emption << sub_prio_index) | temp_interrupt_prio_setup->sub_priority;
  }

	return final_priority_val;
}

/// @brief Disable interrupt registered in the NVIC before
/// @param interrupt_index 
void Interrupt_disable(uint8_t interrupt_index){
  uint8_t is_interrupt_registered = 1;
	Interrupt_prio_setup *temp_interrupt_prio_setup = &interrupt_priority_conf[interrupt_index];
	
  is_interrupt_registered  = GET_INTERRUPT_REGISTERED_STATUS(temp_interrupt_prio_setup->interrupt_instance);
  if(is_interrupt_registered == 1){
    DISABLE_INTERRUPT(temp_interrupt_prio_setup->interrupt_instance);
  }
}

/// @brief Register the interrupt to NVIC
/// @param interrupt_index 
void Interrupt_enable(uint8_t interrupt_index){
  uint8_t is_interrupt_registered = 1;
	Interrupt_prio_setup *temp_interrupt_prio_setup = &interrupt_priority_conf[interrupt_index];
	
  is_interrupt_registered  = GET_INTERRUPT_REGISTERED_STATUS(temp_interrupt_prio_setup->interrupt_instance);

  if(is_interrupt_registered == 0){
    ENABLE_INTERRUPT(temp_interrupt_prio_setup->interrupt_instance);
  }
}
