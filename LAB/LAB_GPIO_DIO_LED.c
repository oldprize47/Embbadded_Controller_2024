/********************************************************************************
* @author  SangheonPark
* @Mod		 2024-09-13	
* @brief   Toggle LED with Button
* 
*******************************************************************************/

#include "ecRCC2.h"
#include "ecGPIO2.h"
#include "ecPinNames.h"

#define LED_PIN PA_5
#define BUTTON_PIN PC_13

// Initialiization 
void setup(void) {
	RCC_HSI_init();
	// Initialize the push button pin to the desired value:
	GPIO_init(BUTTON_PIN, INPUT);
	GPIO_pupd(BUTTON_PIN, PULL_UP);
	
	// Initialize the LED pin to the desired value:
	GPIO_init(LED_PIN, OUTPUT);
	GPIO_otype(LED_PIN,OPENDRAIN);
	GPIO_pupd(LED_PIN,PULL_UP);
	GPIO_ospeed(LED_PIN, MEDIUM_SPEED);
}
	
int main(void) { 
 	setup();
	int debouncing_Switch = 0;														// Variables for debounceing
	while(1){
		if(GPIO_read(BUTTON_PIN)){													// If you don't press the button		
			debouncing_Switch = 1;
			for(int i = 0; i < 100000; i++); 							 		// Repeat for debouncing
		} 
		
		if(!(GPIO_read(BUTTON_PIN)) && debouncing_Switch){ 	// When the button is pressed, the If statement is executed
			GPIO_write(LED_PIN, !(GPIO_read(LED_PIN)));		 		// Current LED Status Inversion
			debouncing_Switch = 0;
		}
	}
}
