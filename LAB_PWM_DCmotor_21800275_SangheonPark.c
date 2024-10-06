/**
******************************************************************************
* @author  SangheonPark
* @Mod		 2024-10-06 by PSH
* @brief   Embedded Controller:  LAB_PWM _DCmoter
* 
******************************************************************************
*/
#include "ecSTM32F4v2.h"
#include <stdlib.h>

#define BUTTON_PIN 	PC_13
#define DIR_PIN 		PC_2
#define PWM_PIN    	PA_0
#define DIR					0 

int count = 0;
int Pause = 0;

void setup(void);
	
int main(void) { 	
	
	setup();
	// Inifinite Loop ----------------------------------------------------------
	while(1){}
}

void TIM3_IRQHandler(void){
	
	if(is_UIF(TIM3)){ // update interrupt flag
		if(Pause){
			PWM_duty(PWM_PIN, 0);			// Pause
		}else if(count < 4){
			PWM_duty(PWM_PIN, 0.53); 	// Slow
		}else{
			PWM_duty(PWM_PIN, 1);			// Fast
		}
		if(!Pause) count++;
		if(count > 7) count = 0;
	}
	clear_UIF(TIM3);   // clear by writing 0 
} 

void EXTI15_10_IRQHandler(void) {
	
	if (is_pending_EXTI(BUTTON_PIN)) {
		Pause = !Pause;										// toggle Pause
		for(int i = 0; i < 1000000; i++);	// delay for debouncing
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
	
	// Direction Pin init
	GPIO_init(DIR_PIN,OUTPUT);
	GPIO_otype(DIR_PIN,PUSHPULL);
	GPIO_write(DIR_PIN, DIR);
	
	//PWM_PIN init
	PWM_init(PWM_PIN);     					// GPIOA 0 ALTERNATE function
	PWM_period(PWM_PIN, 1);					// msec = 1 (Only range 1 to 655 is allowed)
	
	// Timer init
	TIM_UI_init(TIM3, 500);						// msec = 500 (Only range 1 to 655 is allowed)
	
	EXTI_disable(BUTTON_PIN);											
	EXTI_init(BUTTON_PIN, FALL, 0);						// Pin Type, Trig Type(Falling, Rising, Both), Priority
	EXTI_enable(BUTTON_PIN);
}