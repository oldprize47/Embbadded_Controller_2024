/**
******************************************************************************
* @author  SangheonPark
* @Mod	   2024-10-06 by PSH
* @brief   Embedded Controller:  EC_HAL
*
******************************************************************************
*/


#include "ecSTM32F4v2.h"

/* Timer Configuration */

// Default Setting:  1 msec of TimerUEV with Counter_Clk 100kHz / PSC=840-1, ARR=100-1
void TIM_init(TIM_TypeDef* TIMx){     
    // Previous version:  void TIM_init(TIM_TypeDef* TIMx, uint32_t msec) 	
    // 1. Enable Timer CLOCK
	if(TIMx ==TIM1) RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	else if(TIMx ==TIM2) RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	else if(TIMx ==TIM3) RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	else if(TIMx ==TIM4) RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	else if(TIMx ==TIM5) RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	else if(TIMx ==TIM9) RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
	else if(TIMx ==TIM10) RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
	else if(TIMx ==TIM11) RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
	// repeat for TIM4, TIM5, TIM9, TIM11
	
	
    // 2. Set CNT period
	uint32_t msec=1;
	TIM_period_ms(TIMx, msec); 
	
	
    // 3. CNT Direction
	TIMx->CR1 &= ~(1 << 4);					// Upcounter	
	
    // 4. Enable Timer Counter
	TIMx->CR1 |= TIM_CR1_CEN;		
}

// Timer Update Event Period  1~6000 usec  with 1MHz Couter / ARR=1* usec
void TIM_period_us(TIM_TypeDef *TIMx, uint32_t usec){   
	//	Q. Which combination of PSC and ARR for msec unit?
	// PSC = 840 - 1, ARR = 100 - 1; PSC = 84 - 1, ARR = 1000 - 1; => (PSC+1)(ARR+1) = 84000
    // 	Q. What are the possible range (in sec ?)
	// 0.01ms(100kHz, ARR = 1) to 655 msec (ARR = 0xFFFF)
	// 0.01ms(100kHz, ARR = 1) to 40,000,000 msec (ARR = 0xFFFF FFFF)
	

	// 1us(1MHz, ARR=1) to 65msec (ARR=0xFFFF)
	uint16_t PSCval;
	uint32_t Sys_CLK;

	
	if((RCC->CFGR & RCC_CFGR_SW_PLL) == RCC_CFGR_SW_PLL)
		Sys_CLK = 84000000;
	
	else if((RCC->CFGR & RCC_CFGR_SW_HSI) == RCC_CFGR_SW_HSI) 
		Sys_CLK = 16000000;
	
	
	if (TIMx == TIM2 || TIMx == TIM5){
		uint32_t ARRval;
		
		PSCval = Sys_CLK/1000000;						// 84 or 16	--> f_cnt = 1MHz
		ARRval = usec;						// ARRval= 1*usec
		TIMx->PSC = PSCval - 1;
		TIMx->ARR = ARRval - 1;				
	}
	else{
		uint16_t ARRval;

		PSCval = Sys_CLK/1000000;						// 84 or 16	--> f_cnt = 1MHz
		ARRval = usec;					// ARRval= 1*usec
		TIMx->PSC = PSCval - 1;
		TIMx->ARR = ARRval - 1;
	}			
}


// Timer Update Event Period  1~600 msec  with 100kHz Couter / ARR=100*msec
void TIM_period_ms(TIM_TypeDef* TIMx, uint32_t msec){ 
	
	//	Q. Which combination of PSC and ARR for msec unit?
	// // PSC = 840 - 1, ARR = 100 - 1; PSC = 84 - 1, ARR = 1000 - 1; => (PSC+1)(ARR+1) = 84000
	// 
	// 	Q. What are the possible range (in msec ?)
    // 0.02ms(50kHz, ARR=1) to 1.3sec (ARR=0xFFFF)
	//uint32_t prescaler = 1680;
	// 0.1ms(10kHz, ARR = 1) to 6.5sec (ARR = 0xFFFF)
	
	uint16_t PSCval;
	uint32_t Sys_CLK;
	
	if((RCC->CFGR & RCC_CFGR_SW_PLL) == RCC_CFGR_SW_PLL )
		 Sys_CLK = 84000000;
	
	else if((RCC->CFGR & RCC_CFGR_SW_HSI) == RCC_CFGR_SW_HSI) 
		Sys_CLK = 16000000;
	
	
	if (TIMx == TIM2 || TIMx == TIM5){
		uint32_t ARRval;
		PSCval = Sys_CLK/100000;		// 840 or 160	--> PSC_clk=f_cnt = 100kHz
		ARRval = 100*msec;		// 100kHz*msec,  ARRval=100 for 1msec
		TIMx->PSC = PSCval-1;
		TIMx->ARR = ARRval-1;
	}
	else{
		uint16_t ARRval;

		PSCval = Sys_CLK/100000;									
		ARRval = 100*msec;
		TIMx->PSC = PSCval-1;
		TIMx->ARR = ARRval-1;
	}
}

// msec = 1 to 655
void TIM_period(TIM_TypeDef* TIMx, uint32_t msec){
	TIM_period_ms(TIMx, msec);
}



// Update Event Interrupt
void TIM_UI_init(TIM_TypeDef* TIMx, uint32_t msec){
    // 1. Initialize Timer	
	TIM_init(TIMx);
	
	
	TIM_period(TIMx, msec);
	
    // 2. Enable Update Interrupt
	TIM_UI_enable(TIMx);
	
    // 3. NVIC Setting
	uint32_t IRQn_reg =0;
	if(TIMx == TIM1)       IRQn_reg = TIM1_UP_TIM10_IRQn;
	else if(TIMx == TIM2)  IRQn_reg = TIM2_IRQn;
	else if(TIMx == TIM3)  IRQn_reg = TIM3_IRQn;
	else if(TIMx == TIM4)  IRQn_reg = TIM4_IRQn;
	else if(TIMx == TIM5)  IRQn_reg = TIM5_IRQn;
	else if(TIMx == TIM9)  IRQn_reg = TIM1_BRK_TIM9_IRQn;
	else if(TIMx == TIM10)  IRQn_reg = TIM1_UP_TIM10_IRQn;
	else if(TIMx == TIM11)  IRQn_reg = TIM1_TRG_COM_TIM11_IRQn;
	// repeat for TIM3, TIM4, TIM5, TIM9, TIM10, TIM11
	
	NVIC_EnableIRQ(IRQn_reg);				
	NVIC_SetPriority(IRQn_reg,2);
}



void TIM_UI_enable(TIM_TypeDef* TIMx){
	TIMx->DIER |= 1;			// Enable Timer Update Interrupt		
}

void TIM_UI_disable(TIM_TypeDef* TIMx){
	TIMx->DIER &= ~1;				// Disable Timer Update Interrupt		
}

uint32_t is_UIF(TIM_TypeDef *TIMx){
	return TIMx->SR & TIM_SR_UIF_Msk;
}

void clear_UIF(TIM_TypeDef *TIMx){
	TIMx->SR &= ~TIM_SR_UIF_Msk;
}
