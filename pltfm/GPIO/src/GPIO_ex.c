#include "GPIO_ex.h"

//APIs for Exception task
/****************************************
 * These APIs would handle task from External Interrupt 
*/

void EXTI0_IRQHandler(void){
	//Configure EXT mask pin
  EXTI->PR |= ((uint8_t)0x01) << 0; //EXTI_Line0 Pending bit would be cleared by writing 1 to bit 0 of PR register
	
  #ifdef User_define_EXTI_Callback
    EXTI_0_CallBack();
  #endif 
}
void EXTI1_IRQHandler(void){
  ///Function would be configured in the future if the corresponding EXTI line is in used 
}
void EXTI2_IRQHandler(void){
  ///Function would be configured in the future if the corresponding EXTI line is in used 
}
void EXTI3_IRQHandler(void){
  ///Function would be configured in the future if the corresponding EXTI line is in used 
}

void EXTI4_IRQHandler(void){
  ///Function would be configured in the future if the corresponding EXTI line is in used 
} 

