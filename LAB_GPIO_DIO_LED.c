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
	// GPIO_otype(LED_PIN,OPENDRAIN);
	GPIO_pupd(LED_PIN,PULL_UP);
	GPIO_ospeed(LED_PIN, MEDIUM_SPEED);
}
	
int main(void) { 
 	setup();
	
	while(1){
		if(!(GPIO_read(BUTTON_PIN))){						// When the button is pressed, the If statement is executed
			GPIO_write(LED_PIN, !(GPIO_read(LED_PIN)));		// Current LED Status Inversion
			for(int i = 0; i < 300000; i++); 				// Repeat for delay
		}
	}
}
