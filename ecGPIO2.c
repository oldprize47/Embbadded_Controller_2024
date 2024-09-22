/*----------------------------------------------------------------
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SangheonPark
Created          : 2024-09-10
Modified         : 2024-09-22 
Language/ver     : C++ in Keil uVision

Description      : Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/



#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "ecGPIO2.h"

PinName_t LED_PIN_decoder[4] = {0};	// Seven segment LED Pin Array

void GPIO_init(PinName_t pinName, uint32_t mode){     
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName, &Port, &pin);
	
	// mode  : Input(0), Output(1), AlterFunc(2), Analog(3)   
	if (Port == GPIOA)
		RCC_GPIOA_enable();
	if (Port == GPIOB)
		RCC_GPIOB_enable();
	if (Port == GPIOC)
		RCC_GPIOC_enable();
	if (Port == GPIOD)
		RCC_GPIOD_enable();
	if (Port == GPIOE)
		RCC_GPIOE_enable();
	if (Port == GPIOH)
		RCC_GPIOH_enable();


	// You can also make a more general function of
	// void RCC_GPIO_enable(GPIO_TypeDef *Port); 

	GPIO_mode(pinName, mode);
}

void GPIO_write(PinName_t pinName, int Output){

	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);

	Port->ODR &= ~(1UL<<(pin));
	Port->ODR |= Output<<(pin); 
}

int GPIO_read(PinName_t pinName){
	//[TO-DO] YOUR CODE GOES HERE
	//[TO-DO] YOUR CODE GOES HERE
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	
	return ((Port->IDR) >> pin) & 1; //[TO-DO] YOUR CODE GOES HERE	
}

// GPIO Mode          : Input(00), Output(01), AlterFunc(10), Analog(11)
void GPIO_mode(PinName_t pinName, uint32_t mode){
   GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
   Port->MODER &= ~(3UL<<(2*pin));     
   Port->MODER |= mode<<(2*pin);    
}


// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
void GPIO_ospeed(PinName_t pinName, int speed){
	//[TO-DO] YOUR CODE GOES HERE
	//[TO-DO] YOUR CODE GOES HERE
	 GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	 Port->OSPEEDR &= ~(3UL<<(2*pin));     
   Port->OSPEEDR |= speed<<(2*pin); 
}

// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
void GPIO_otype(PinName_t pinName, int type){
   	//[TO-DO] YOUR CODE GOES HERE
	//[TO-DO] YOUR CODE GOES HERE
	 GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	 Port->OTYPER &= ~(type<<(pin));     
   Port->OTYPER |= type<<(pin); 
}

// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
void GPIO_pupd(PinName_t pinName, int pupd){
   	//[TO-DO] YOUR CODE GOES HERE
	//[TO-DO] YOUR CODE GOES HERE
	 GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	 Port->PUPDR &= ~(3UL<<(2*pin));     
   Port->PUPDR |= pupd<<(2*pin); 
}

void sevensegment_display_init(PinName_t pinNameA, PinName_t pinNameB, PinName_t pinNameC, PinName_t pinNameD){
	
	LED_PIN_decoder[0] = pinNameA;
	LED_PIN_decoder[1] = pinNameB;
	LED_PIN_decoder[2] = pinNameC;
	LED_PIN_decoder[3] = pinNameD;
	for(int i = 0; i < 4; i++){
		GPIO_init(LED_PIN_decoder[i], OUTPUT);
		GPIO_otype(LED_PIN_decoder[i],PUSHPULL);
		GPIO_pupd(LED_PIN_decoder[i],NOPUPD);
		GPIO_ospeed(LED_PIN_decoder[i], MEDIUM_SPEED);
	}
}

void sevensegment_display(uint8_t  num){
	
	// Decimal to Binary conversion
	for(int i = 3; i >= 0; i--){						
		GPIO_write(LED_PIN_decoder[i], num%2);	// Put the remainder in from the end to the first
		num /= 2;																
	}
}
