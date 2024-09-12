/*----------------------------------------------------------------
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SangheonPark
Created          : 2024-09-10
Modified         : 2024-09-10
Language/ver     : C in Keil uVision

Description      : Distributed to Students for Simple Tutorials
/----------------------------------------------------------------*/


#include "stm32f411xe.h"
#include "ecPinNames.h"

#ifndef __ECSIMPLE_H
#define __ECSIMPLE_H

#define INPUT  0x00
#define OUTPUT 0x01
#define AF     0x02
#define ANALOG 0x03

#define HIGH 1
#define LOW  0

#define LED_PIN 	5
#define BUTTON_PIN 13

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
	 

void GPIO_init(PinName_t pinName, uint32_t mode);
void GPIO_mode(PinName_t pinName, uint32_t mode);
void GPIO_write(PinName_t pinName, int Output);
int  GPIO_read(PinName_t pinName);

void RCC_HSI_init(void);
void RCC_GPIOA_enable(void);
void RCC_GPIOB_enable(void);
void RCC_GPIOC_enable(void);
 


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
