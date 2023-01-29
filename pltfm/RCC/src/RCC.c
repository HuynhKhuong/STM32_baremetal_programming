#include "RCC.h"

#define RVR_max_value (uint32_t)0x00FFFFFF

uint8_t is_ex_occur = 0;
/*
  @brief: delaye fucntion in ms
  @input: number of ms units 
*/
void Systick_Delay(const uint32_t ms_delay_u32){
  //Condition check if input exceeds the reload value
  if(ms_delay_u32 > RVR_max_value) return;

  //Delay logic
  uint32_t ms_counter_u32 = 0;
  while(ms_counter_u32 < ms_delay_u32){
    if(is_ex_occur){
      ms_counter_u32++;
      is_ex_occur = 0;
    } 
  }
}
