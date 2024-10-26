/*******************************************************
		Name: Sangheon			ID: 21800275
		Date: 2024-10-15
		TEST 1
********************************************************/
#include "ecSTM32F4v2.h"
#include <stdlib.h>

// State definition
#define S0  0
#define S1  1
#define S2  2
#define S3  3
#define S4  4
#define S5  5

#define BUTTON_PIN 	PC_13
#define DIR_PIN 	PC_2
#define PWM_PIN    	PA_0
#define IR_PIN    	PB_1


int DIR = 0;
int CCW = 1;
unsigned int count = 0;
int cnt = 0;
unsigned int rate = 0;	//for 1sec 
double duty = 0;
double targetPWM = 0.0;
int pressed_flag = 0;

void setup(void);
void updateState();
int main(void) { 	
	
	setup();
	// Inifinite Loop ----------------------------------------------------------
	while(1){}
}

void TIM3_IRQHandler(void){
	if(is_UIF(TIM3)){ // update interrupt flag
		
		if(pressed_flag){
			updateState();
			pressed_flag = 0;
		}
		
		// DC motor
		if(count %3 == 0){					// count == 0, 3
			targetPWM = 0;						
			sevensegment_Off();
		}else if(count %3 == 1){		// count == 1, 4
			targetPWM = 0.53;					// slow
		}else if(count %3 == 2){		// count == 2, 5
			targetPWM = 1;						// Fast
		}
		
		duty = fabs(DIR - targetPWM);
		PWM_duty(PWM_PIN, duty);
		
		// 7- segment
		if(count < 3){
			if((count % 3) > rate){
				cnt++;
				rate = 1;
			}else rate = 0;
		}else{
			if((count % 3) > rate){
				cnt--;
				rate = 1;
			}else rate = 0;
		}

		if(cnt > 9) cnt = 0;
		else if(cnt < 0) cnt = 9;
		if((count %3) != 0)sevensegment_display(cnt % 10);
		

		
		clear_UIF(TIM3);   // clear by writing 0 
	}
	
} 
void EXTI1_IRQHandler(void) {
	if (is_pending_EXTI(IR_PIN)){
		count = 0;
		GPIO_write(DIR_PIN, DIR);
		PWM_duty(PWM_PIN, 1);			// OFF
		sevensegment_Off();
		cnt = 0;										
		rate = 0;
		clear_pending_EXTI(IR_PIN);
	}
}

void EXTI15_10_IRQHandler(void) {
	
	if (is_pending_EXTI(BUTTON_PIN)) {
		pressed_flag = 1;
		clear_pending_EXTI(BUTTON_PIN);
	}
	
}

void updateState(){
		count++;										// count
		if(count > 5) count = 0;
		if(count %3 == 0){
			DIR = !DIR;								// Change DIR
			GPIO_write(DIR_PIN, DIR);
		}
}

// Initialiization 
void setup(void)
{	
	// Initialiization --------------------------------------------------------
	RCC_PLL_init();       // System Clock = 84MHz
	SysTick_init();
	
	// BUTTON_PIN init
	GPIO_init(BUTTON_PIN,INPUT);
	GPIO_pupd(BUTTON_PIN,PULL_DOWN);
	
	//IR_PIN init
	GPIO_init(IR_PIN,INPUT);
	GPIO_pupd(IR_PIN,PULL_DOWN);
	
	//Sevensegment_PIN init
	sevensegment_display_init(PA_7, PB_6, PC_7, PA_9);  // Decoder input A, B, C, D for Problem 1
	
	// Direction Pin init
	GPIO_init(DIR_PIN,OUTPUT);
	GPIO_otype(DIR_PIN,PUSHPULL);
	GPIO_write(DIR_PIN, DIR);
	
	//PWM_PIN init
	PWM_init(PWM_PIN);     					// GPIOA 0 ALTERNATE function
	PWM_period(PWM_PIN, 1);					// msec = 1 (Only range 1 to 655 is allowed)
	
	// Timer init
	TIM_UI_init(TIM3, 500);						// msec = 500 (Only range 1 to 655 is allowed)
	
	//EXTI_BUTTON_PIN									
	EXTI_init(BUTTON_PIN, RISE, 0);		// Pin Type, Trig Type(Falling, Rising, Both), Priority
	EXTI_enable(BUTTON_PIN);
	
	//EXTI_IR_PIN
	EXTI_init(IR_PIN, RISE, 0);			// Pin Type, Trig Type(Falling, Rising, Both), Priority
	EXTI_enable(IR_PIN);
}