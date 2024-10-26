#include "stm32f411xe.h"
#include "ecSTM32F4v2.h"

#define BUTTON_PIN PC_13
void setup(void);

int main(void)
{
	// Initialiization --------------------------------------------------------
	setup();

	/******************************************FULL********************************************/
	for(int i = 1; i <= 10; i ++){
		Stepper_step(2048, 1, FULL); // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	}
	for(int i = 1; i <= 10; i ++){
		Stepper_step(2048, 0, FULL); // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	}
	
	for(int i = 13; i <= 16; i ++){
		Stepper_setSpeed(i);
		Stepper_step(2048/2, 1, FULL); // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	}

	for(int i = 2; i >= 1; i--){
		Stepper_setSpeed(i);
		Stepper_step(2048/4, 0, FULL); // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	}

	/******************************************HALF********************************************/
	Stepper_setSpeed(4);
	for(int i = 1; i <= 10; i ++){
		Stepper_step(2048*2, 1, HALF); // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	}
	for(int i = 1; i <= 10; i ++){
		Stepper_step(2048*2, 0, HALF); // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	}
	
	for(int i = 26; i <= 32; i ++){
		Stepper_setSpeed(i);
		Stepper_step(2048, 1, HALF); // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	}

	for(int i = 2; i >= 1; i--){
		Stepper_setSpeed(i);
		Stepper_step(2048/2, 0, HALF); // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
	}

	

	// Inifinite Loop ----------------------------------------------------------
	while (1)
	{
	}
}

// Initialiization
void setup(void)
{

	RCC_PLL_init(); // System Clock = 84MHz
	SysTick_init(); // Systick init

	EXTI_init(BUTTON_PIN, FALL, 0); // External Interrupt Setting
	GPIO_init(BUTTON_PIN, INPUT);	// GPIOC pin13 initialization

	Stepper_init(PB_10, PB_4, PB_5, PB_3); // Stepper GPIO pin initialization
	Stepper_setSpeed(2);				   //  set stepper motor speed
}

void EXTI15_10_IRQHandler(void)
{
	if (is_pending_EXTI(BUTTON_PIN))
	{
		Stepper_stop();
		clear_pending_EXTI(BUTTON_PIN); // cleared by writing '1'
	}
}