/*----------------------------------------------------------------
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : SangheonPark
Created          : 2024-09-28
Modified         : 2024-09-28 
Language/ver     : C++ in Keil uVision

Description      : Distributed to Students for LAB_EXTI
/----------------------------------------------------------------*/

#ifndef __EC_EXTI2_H
#define __EC_EXTI2_H

#include "ecSTM32F4v2.h"

#define FALL 0
#define RISE 1
#define BOTH 2

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

void EXTI_init(PinName_t pinName, int trig_type,int priority);
void EXTI_enable(PinName_t pinName);
void EXTI_disable(PinName_t pinName);
uint32_t is_pending_EXTI(PinName_t pinName);
void clear_pending_EXTI(PinName_t pinName);

#ifdef __cplusplus
}
#endif /* __cplusplus */
	 
#endif // __EC_EXTI2_H
