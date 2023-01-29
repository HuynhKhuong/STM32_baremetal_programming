#include "RCC.h" 
#include <stdio.h>

#define HSERDY_bit_pos RCC_CR_HSERDY
#define HSIRDY_bit_pos RCC_CR_HSIRDY
#define PLLRDY_bit_pos RCC_CR_PLLRDY

#define PLLON_bit_pos RCC_CR_PLLON

#define HSERDY_bit_index 17
#define HSIRDY_bit_index 1 
#define PLLRDY_bit_index 25

#define SYSCLOCKSRC_Mask RCC_CFGR_SWS
#define SYSCLOCKSW_Mask RCC_CFGR_SW

static uint8_t is_HSE_src_available(void){
	const uint32_t temp_reg_value_u32 = RCC->CR;
	const uint8_t bit_value_u8 = (temp_reg_value_u32 & HSERDY_bit_pos) >> HSERDY_bit_index;
	return bit_value_u8;
}


static uint8_t is_HSI_src_available(void){
	const uint32_t temp_reg_value_u32 = RCC->CR;
	const uint8_t bit_value_u8 = (temp_reg_value_u32 & HSIRDY_bit_pos) >> HSIRDY_bit_index;
	return bit_value_u8;
}

static uint8_t is_PLL_src_available(void){
	const uint32_t temp_reg_value_u32 = RCC->CR;
	const uint8_t bit_value_u8 = (temp_reg_value_u32 & PLLRDY_bit_pos) >> PLLRDY_bit_index;
	return bit_value_u8;
}


const RCC_conf clock_tree_conf_cst[NUM_OF_CLKTREE_INSTANCE]={
  {
		/*
			@brief: configration 1
			Source: HSE 8MHz -> PLL -> 72MHz -> AHB = 72MHz, SYSCLK = AHB/8, APB1 = 36MHz (Max), APB2 = 72MHz(Max)
		*/
		&(RCC->CR),
		&(RCC->CFGR),
		//PLL configuration
		{
			PLL_HSE, //PLL source
			PLLx9 //x9
		}, 
		SYS_PLL, //system clock source is PLL
		AHB_DIV_2, //System clock freq = 72/8 = 9MHz
		APB_DIV_2,
		APB_DIV_2,
	}
};

/*
	@brief: Function Configurtion the clock tree, including: SYSCLOCK freq, AHB freq, APBx freq, clock input is PLL
	@input: profile index, taken from the clock_tree_conf_cst struct
	@ouptut: void
*/
void Clock_tree_init(uint8_t clock_tree_profile){

  const RCC_conf temp_clock_tree_config = clock_tree_conf_cst[clock_tree_profile];

	//Clock source configuration
	//Further configuration is needed for PLL only, HSI & HSE doesn't need any configuration but switching the source	 
	uint32_t current_sysclock_src_u32 = (*temp_clock_tree_config.clock_config_reg & SYSCLOCKSRC_Mask) >> 2; //bit index is 2-3

	switch(temp_clock_tree_config.systick_clock_src){
		case SYS_PLL: //PLL source, configure PLL
			//check if hardware source is okay
			if(!is_PLL_src_available()) break;

			if(current_sysclock_src_u32 == SYS_PLL){ //if current SYSCLOCKSRC is PLL, switch to other clock to disable PLL
				//Switch to HSI temporarily
				*temp_clock_tree_config.clock_config_reg &= ~SYSCLOCKSW_Mask;
			}

			*temp_clock_tree_config.clock_src_control_reg &= ~PLLON_bit_pos; //disable PLL to configure PLL component
			*temp_clock_tree_config.clock_config_reg |= (uint32_t)temp_clock_tree_config.clock_src_PLL.PLL_Clock_src << 16; //bit 16 is bit clock src
			*temp_clock_tree_config.clock_config_reg |= (uint32_t)temp_clock_tree_config.clock_src_PLL.Multiplier << 18; //bit 18-21 are multiplier configuration

			//switch back to PLL
			*temp_clock_tree_config.clock_config_reg |= (uint32_t)temp_clock_tree_config.systick_clock_src;
			*temp_clock_tree_config.clock_src_control_reg |= (uint32_t)PLLON_bit_pos;
			break;
		case SYS_HSI:
			//Enable condition Check for PLL configuration
			if(!is_HSI_src_available()) break;
			*temp_clock_tree_config.clock_config_reg |= (uint32_t)temp_clock_tree_config.systick_clock_src;
			break;
		case SYS_HSE: 
			if(!is_HSE_src_available()) break;
			*temp_clock_tree_config.clock_config_reg |= (uint32_t)temp_clock_tree_config.systick_clock_src;
			break;
	}

	//AHB prescaler configuration
	*temp_clock_tree_config.clock_config_reg |= (uint32_t)temp_clock_tree_config.AHB_prescaler << 4; //bit 4-7 are AHB_prescaler
	*temp_clock_tree_config.clock_config_reg |= (uint32_t)temp_clock_tree_config.APB1_prescaler << 8; //bit 8-10 are APB1_prescaler
	*temp_clock_tree_config.clock_config_reg |= (uint32_t)temp_clock_tree_config.APB2_prescaler << 11; //bit 11-13 are APB2_prescaler
}
