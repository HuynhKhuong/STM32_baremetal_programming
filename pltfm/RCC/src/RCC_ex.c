#include "RCC_ex.h"

uint8_t Systick_Callback(void){
  //This function would be called in Systick Interrupt handler
	
  return 1;
}


//Systick Handler is a wrapper to call Systick_Callback
void SysTick_Handler(void){
	Systick_Callback();
}
