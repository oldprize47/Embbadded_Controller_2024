/*----------------------------------------------------------------
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SangheonPark
Created          : 2024-09-10
Modified         : 2024-09-22 
Language/ver     : C++ in Keil uVision

Description      : Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/



#include "ecSTM32F4v2.h"

PinName_t LED_PIN_display[4] = {0};	// Seven segment LED Pin Array
PinName_t LED_PIN_decoder[8] = {0};	// Seven segment decoder LED Pin Array

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
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	
	return ((Port->IDR) >> pin) & 1;
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

	 GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	 Port->OSPEEDR &= ~(3UL<<(2*pin));     
   Port->OSPEEDR |= speed<<(2*pin); 
}

// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
void GPIO_otype(PinName_t pinName, int type){

	 GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	 Port->OTYPER &= ~(type<<(pin));     
   Port->OTYPER |= type<<(pin); 
}

// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
void GPIO_pupd(PinName_t pinName, int pupd){

	 GPIO_TypeDef * Port;
   unsigned int pin;
   ecPinmap(pinName,&Port,&pin);
	 Port->PUPDR &= ~(3UL<<(2*pin));     
   Port->PUPDR |= pupd<<(2*pin); 
}

void sevensegment_display_init(PinName_t pinNameA, PinName_t pinNameB, PinName_t pinNameC, PinName_t pinNameD){
	
	LED_PIN_display[3] = pinNameA;
	LED_PIN_display[2] = pinNameB;
	LED_PIN_display[1] = pinNameC;
	LED_PIN_display[0] = pinNameD;
	for(int i = 0; i < 4; i++){
		GPIO_init(LED_PIN_display[i], OUTPUT);
		GPIO_otype(LED_PIN_display[i],PUSHPULL);
		GPIO_pupd(LED_PIN_display[i],NOPUPD);
		GPIO_ospeed(LED_PIN_display[i], MEDIUM_SPEED);
	}
}

void sevensegment_display(uint8_t  num){
	
	// Decimal to Binary conversion
	for(int i = 3; i >= 0; i--){						
		GPIO_write(LED_PIN_display[i], num%2);	// Put the remainder in from the end to the first
		num /= 2;																
	}
}

void sevensegment_Off(){
	
	// Decimal to Binary conversion
	for(int i = 3; i >= 0; i--){						
		GPIO_write(LED_PIN_display[i], 1);	// Put the remainder in from the end to the first															
	}
}

void sevensegment_decoder_init(PinName_t pin_a, PinName_t pin_b, PinName_t pin_c, PinName_t pin_d, PinName_t pin_e, PinName_t pin_f, PinName_t pin_g, PinName_t pin_dp){
	LED_PIN_decoder[0] = pin_a;
	LED_PIN_decoder[1] = pin_b;
	LED_PIN_decoder[2] = pin_c;
	LED_PIN_decoder[3] = pin_d;
	LED_PIN_decoder[4] = pin_e;
	LED_PIN_decoder[5] = pin_f;
	LED_PIN_decoder[6] = pin_g;
	LED_PIN_decoder[7] = pin_dp;
	
	for(int i = 0; i < 8; i++){
		GPIO_init(LED_PIN_decoder[i], OUTPUT);
		GPIO_otype(LED_PIN_decoder[i],PUSHPULL);
		GPIO_pupd(LED_PIN_decoder[i],NOPUPD);
		GPIO_ospeed(LED_PIN_decoder[i], MEDIUM_SPEED);
	}
}
void sevensegment_decoder(uint8_t  num){
	int decoder_number[10][8]={
		{0,0,0,0,0,0,1,1},          //zero
		{1,0,0,1,1,1,1,1},          //one
		{0,0,1,0,0,1,0,1},          //two
		{0,0,0,0,1,1,0,1},          //three
		{1,0,0,1,1,0,0,1},          //four
		{0,1,0,0,1,0,0,1},          //five
		{1,1,0,0,0,0,0,1},          //six
		{0,0,0,1,1,0,1,1},          //seven
		{0,0,0,0,0,0,0,1},          //eight
		{0,0,0,1,1,0,0,1},          //nine
	};
	
	for(int i = 0; i < 8; i++){
		GPIO_write(LED_PIN_decoder[i], decoder_number[num][i]);
	}
}

void LED_toggle(){
	GPIO_TypeDef * Port;
	unsigned int pin;
	ecPinmap(PA_5,&Port,&pin);
	
	// read LED_PIN_Output and toggle
	GPIO_write(PA_5, !(((Port->ODR) >> pin) & 1)); 
}