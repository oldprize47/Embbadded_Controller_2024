/*----------------------------------------------------------------
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SangheonPark
Created          : 2024-09-28
Modified         : 2024-09-28 
Language/ver     : C++ in Keil uVision

Description      : Distributed to Students for LAB_EXTI_SysTick
/----------------------------------------------------------------*/

#ifndef __EC_SYSTICK2_H
#define __EC_SYSTICK2_H

#include "ecSTM32F4v2.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

void SysTick_init(void);
void SysTick_Handler(void);
void SysTick_counter();
void delay_ms(uint32_t msec);
void SysTick_reset(void);
uint32_t SysTick_val(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __EC_SYSTICK2_H
