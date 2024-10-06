/********************************************************************************
*
* @author  SangheonPark
* @Mod		 2024-09-25 by PSH
*
********************************************************************************/



#include "ecSTM32F4v2.h"

#define LED_PIN   PA_5
#define BUTTON_PIN PC_13

void setup(void);
unsigned int cnt = 0;

int main(void) {

	// System CLOCK, GPIO Initialiization ----------------------------------------
	setup();
	
	while (1);
}

void EXTI15_10_IRQHandler(void) {
	
	if (is_pending_EXTI(BUTTON_PIN)) {
		cnt++;
		if (cnt > 9) cnt = 0;
		sevensegment_display(cnt % 10);
		for(int i = 0; i < 1000000; i++);  // delay_ms(1000);
		clear_pending_EXTI(BUTTON_PIN);
	}
	
}


// Initialiization 
void setup(void)
{
	RCC_PLL_init();                         // System Clock = 84MHz
	
	// initialize the LED pin to the desired value:
	sevensegment_display_init(PA_7, PB_6, PC_7, PA_9);  // Decoder input A, B, C, D
	
	// Initialize GPIOC_13 for Input Button
	EXTI_disable(BUTTON_PIN);
	EXTI_init(BUTTON_PIN, FALL, 0);						// Pin Type, Trig Type(Falling, Rising, Both), Priority
	EXTI_enable(BUTTON_PIN);
	
}