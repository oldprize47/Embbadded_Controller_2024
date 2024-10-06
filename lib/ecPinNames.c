/*----------------------------------------------------------------
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SangheonPark
Created          : 2024-09-10
Modified         : 2024-09-10
Language/ver     : C in Keil uVision

Description      : Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/

#include "ecSTM32F4v2.h"

void ecPinmap(PinName_t pinName, GPIO_TypeDef **GPIOx, unsigned int *pin)
{
	
	unsigned int pinNum= pinName & (0x000F);
	*pin=pinNum;

	unsigned int portNum=(pinName>>4);
	
	
	if (portNum==0)
		*GPIOx=GPIOA;
	else if (portNum==1)
		*GPIOx=GPIOB;
	else if (portNum==2)
		*GPIOx=GPIOC;
	else if (portNum==3)
		*GPIOx=GPIOD;
	else if (portNum==4)
		*GPIOx=GPIOE;
//	else if (portNum==5)
//		*GPIOx=GPIOF;
//	else if (portNum==6)
//		*GPIOx=GPIOG;	
	else if (portNum==7)
		*GPIOx=GPIOH;
	else 
		*GPIOx=GPIOA;
}