/**
******************************************************************************
* @author  SangheonPark
* @Mod		 2024-10-06 by PSH
* @brief   Embedded Controller:  LAB_PWM _RCmoter
* 
******************************************************************************
*/
#include "ecSTM32F4v2.h"

#define PWM_PIN    PA_1
#define BUTTON_PIN PC_13

float degree10 = 0.005556;		// 10dgree to duty
float angle = 0.0;						// Unit 10 dgrees Angle size
int dir = 1;									// 1 = Counter clock wise, 0 = clock wise

void setup(void);
	
int main(void) { 	
	
	setup();
	// Inifinite Loop ----------------------------------------------------------
	while(1){
	}
}

void TIM3_IRQHandler(void){
	
	if(is_UIF(TIM3)){ // update interrupt flag 
		PWM_duty(PWM_PIN, 0.025+(degree10*angle));
		angle += dir;
		if(angle >= 18) dir = -1;
		else if(angle <= 0) dir = 1;
	}
	clear_UIF(TIM3);   // clear by writing 0 
} 

void EXTI15_10_IRQHandler(void) {
	
	if (is_pending_EXTI(BUTTON_PIN)) {
		angle = 0;
		PWM_duty(PWM_PIN, 0.025+(degree10*angle));
		clear_pending_EXTI(BUTTON_PIN);
	}
	
}

// Initialiization 
void setup(void)
{	
	// Initialiization --------------------------------------------------------
	RCC_PLL_init();       // System Clock = 84MHz
	
	// BUTTON_PIN init
	GPIO_init(BUTTON_PIN,INPUT);
	GPIO_pupd(BUTTON_PIN,PULL_UP);
	
	//PWM_PIN init
	PWM_init(PWM_PIN);     					// GPIOA 1 ALTERNATE function
	PWM_period(PWM_PIN, 20);				// msec = 20 (Only range 1 to 655 is allowed)
	
	// Timer init
	TIM_UI_init(TIM3, 500);					// msec = 500 (Only range 1 to 655 is allowed)
	
	EXTI_disable(BUTTON_PIN);											
	EXTI_init(BUTTON_PIN, FALL, 0);						// Pin Type, Trig Type(Falling, Rising, Both), Priority
	EXTI_enable(BUTTON_PIN);
}