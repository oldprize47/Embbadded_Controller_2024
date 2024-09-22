/*----------------------------------------------------------------
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SangheonPark
Created          : 2024-09-10
Modified         : 2024-09-22 
Language/ver     : C++ in Keil uVision

Description      : Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/


#ifndef __ECGPIO2_H
#define __ECGPIO2_H

#include "stm32f411xe.h"
#include "ecRCC2.h"
#include "ecPinNames.h"

// MODER
#define INPUT  0x00
#define OUTPUT 0x01
#define AF     0x02
#define ANALOG 0x03

//OTYPER
#define PUSHPULL 0x0
#define OPENDRAIN 0x1

// OSPEEDR
#define LOW_SPEED 0x00
#define MEDIUM_SPEED 0x01
#define FAST_SPEED 0x02
#define HIGH_SPEED 0x03

// PUPDR
#define NOPUPD 0x00
#define PULL_UP 0x01
#define PULL_DOWN 0x02
#define RESERVED 0x03

#define HIGH 1
#define LOW  0

#define LED_PIN_1 PA7
#define LED_PIN_2 PB6
#define LED_PIN_3 PC7
#define LED_PIN_4 PA9
//#define LED_PIN PA_5   //Find LED Port&Pin and Fill the blank	
//#define BUTTON_PIN PC_13//Find BTN Port&Pin and Fill the blank

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */
	 
void GPIO_init(PinName_t pinName, uint32_t mode);     
void GPIO_write(PinName_t pinName, int Output);
int  GPIO_read(PinName_t pinName);
void GPIO_mode(PinName_t pinName, uint32_t mode);
void GPIO_ospeed(PinName_t pinName, int speed);
void GPIO_otype(PinName_t pinName, int type);
void GPIO_pupd(PinName_t pinName, int pupd);
void sevensegment_display_init(PinName_t pinNameA, PinName_t pinNameB, PinName_t pinNameC, PinName_t pinNameD);
void sevensegment_display(uint8_t  num);
void sevensegment_decoder_init(PinName_t pin_a, PinName_t pin_b, PinName_t pin_c, PinName_t pin_d, PinName_t pin_e, PinName_t pin_f, PinName_t pin_g, PinName_t pin_dp);
void sevensegment_decoder(uint8_t  num);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __ECGPIO2_H
