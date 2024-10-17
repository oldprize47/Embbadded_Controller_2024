/*----------------------------------------------------------------
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SangheonPark
Created          : 2024-09-28
Modified         : 2024-09-28 
Language/ver     : C++ in Keil uVision

Description      : Distributed to Students for LAB_EXTI
/----------------------------------------------------------------*/

#include "ecSTM32F4v2.h"


void EXTI_init(PinName_t pinName, int trig_type,int priority){

	GPIO_TypeDef *Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	// SYSCFG peripheral clock enable	
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	//	RCC->APB2ENR |= 1 << 14;
	
	// Connect External Line to the GPIO
	int EXTICR_port;
	if			(Port == GPIOA) EXTICR_port = 0;
	else if	(Port == GPIOB) EXTICR_port = 1;
	else if	(Port == GPIOC) EXTICR_port = 2;
	else if	(Port == GPIOD) EXTICR_port = 3;
	else if	(Port == GPIOE) EXTICR_port = 4;
	else 										EXTICR_port = 7;
	
	SYSCFG->EXTICR[pin/4] &= ~(0b1111 << 4*(pin%4));			// clear 4 bits
	SYSCFG->EXTICR[pin/4] |= (EXTICR_port << 4*(pin%4));		// set 4 bits
	
	
	// Configure Trigger edge
	if (trig_type == FALL) EXTI->FTSR |= 1 << pin;   // Falling trigger enable 
	else if	(trig_type == RISE) EXTI->RTSR |= 1 << pin;   // Rising trigger enable 
	else if	(trig_type == BOTH) {			// Both falling/rising trigger enable
		EXTI->RTSR |= 1 << pin; 
		EXTI->FTSR |= 1 << pin;
	} 
	
	// Configure Interrupt Mask (Interrupt enabled)
	EXTI->IMR  |= 1 << pin;     // not masked
	
	
	// NVIC(IRQ) Setting
	int EXTI_IRQn = 0;
	
	if (pin < 5) 	EXTI_IRQn = pin+6;
	else if	(pin < 10) 	EXTI_IRQn = EXTI9_5_IRQn;
	else 			EXTI_IRQn = EXTI15_10_IRQn;
								
	NVIC_SetPriority(EXTI_IRQn, priority);	// EXTI priority
	NVIC_EnableIRQ(EXTI_IRQn); 	// EXTI IRQ enable
}


void EXTI_enable(PinName_t pinName) {
	GPIO_TypeDef *Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	//Todo Port, Pin Configuration 
	EXTI->IMR |= 1 << pin;     // not masked (i.e., Interrupt enabled)
}
void EXTI_disable(PinName_t pinName) {
	GPIO_TypeDef *Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	//Todo Port, Pin Configuration 
	EXTI->IMR &= ~(1 << pin);     // masked (i.e., Interrupt disabled)
}

uint32_t is_pending_EXTI(PinName_t pinName) {
	GPIO_TypeDef *Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	
	//Todo Port, Pin Configuration 
	uint32_t EXTI_PRx = pin;     	// check  EXTI pending 	
	return ((EXTI->PR >> EXTI_PRx));
}


void clear_pending_EXTI(PinName_t pinName) {
	GPIO_TypeDef *Port;
	unsigned int pin;
	ecPinmap(pinName,&Port,&pin);
	//Todo Port, Pin Configuration 
	EXTI->PR |= 1 << pin;     // clear EXTI pending 
}
