/**
******************************************************************************
* @author  SangheonPark
* @Mod		 2024-09-22 	
* @brief   Embedded Controller:  Tutorial Digital In/Out 7-segment Display
* 
******************************************************************************
*/

#include "stm32f4xx.h"
#include "ecRCC2.h"
#include "ecGPIO2.h"

#define LED_PIN PA_5
#define BUTTON_PIN PC_13

void setup(void);
	
int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	unsigned int cnt = 0;
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		if(GPIO_read(BUTTON_PIN) == 0) cnt++;
//		sevensegment_decoder(cnt % 10);		// for Problem 2
		sevensegment_display(cnt % 10);	// for Problem 1
		if (cnt > 9) cnt = 0;
		for(int i = 0; i < 500000;i++);  // delay_ms(500);
	}
}


// Initialiization 
void setup(void)
{
	RCC_HSI_init();
	GPIO_init(BUTTON_PIN, INPUT);  		// calls RCC_GPIOC_enable()
	GPIO_pupd(BUTTON_PIN, PULL_UP);
	
	// initialize the LED pin to the desired value:
//	sevensegment_decoder_init(PA_5, PA_6, PA_7, PB_6, PC_7, PA_9, PA_8, PB_10);	// sevensegment input a, b, c, d, e, f, g, dp for Problem 2
	sevensegment_display_init(PA_7, PB_6, PC_7, PA_9);  // Decoder input A, B, C, D for Problem 1
	
}