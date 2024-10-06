/********************************************************************************
* @author  SangheonPark
* @Mod		2024-09-13	
* @brief   Change LED with Button
* 
*******************************************************************************/

#include "ecRCC2.h"
#include "ecGPIO2.h"
#include "ecPinNames.h"

#define BUTTON_PIN PC_13
PinName_t LED_PIN[4] = {PA_5, PA_6, PA_7, PB_6};	// LED Pin Array

// Initialiization 
void setup(void) {
	RCC_HSI_init();
	// Initialize the push button pin to the desired value:
	GPIO_init(BUTTON_PIN, INPUT);
	GPIO_pupd(BUTTON_PIN, PULL_UP);

	// initialize the LED pin to the desired value:
	for(int i = 0; i < 4; i++){
		GPIO_init(LED_PIN[i], OUTPUT);
		GPIO_otype(LED_PIN[i],PUSHPULL);
		GPIO_pupd(LED_PIN[i],PULL_UP);
		GPIO_ospeed(LED_PIN[i], MEDIUM_SPEED);
	}
}
	
int main(void) { 
 	setup();
	int pinNum = 0;
	int debouncing_Switch = 0;														// Variables for debounceing
	GPIO_write(LED_PIN[pinNum], HIGH);										// Start with port A number 5 pin on

	while(1){
		if(GPIO_read(BUTTON_PIN)){													// When I let go of the button
			debouncing_Switch = 1;
			for(int i = 0; i < 100000; i++); 									// Repeat for debouncing
		}
		
		if(!(GPIO_read(BUTTON_PIN)) && debouncing_Switch){	// If(push_Button)
			GPIO_write(LED_PIN[pinNum], LOW);									// Turn off the previous LED	
			pinNum++;																					// Change to next pin
			if(pinNum >= 4) pinNum = 0;												// Range is 0 to 3
			GPIO_write(LED_PIN[pinNum], HIGH);								// Turn on the changed LED
			debouncing_Switch = 0;
		}
	}
}