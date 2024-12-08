#include "stm32f411xe.h"
#include "ecSTM32F4v2.h"
			
#ifndef __EC_STEPPER2_H
#define __EC_STEPPER2_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

//State mode
#define HALF 0
#define FULL 1	 
	 
/* Stepper Motor */
//stepper motor function

typedef struct{
	PinName_t pin[4];
	uint32_t _step_num;
} Stepper_t;

	 
void Stepper1_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4);
void Stepper1_setSpeed(long whatSpeed);
void Stepper1_step(uint32_t steps, uint32_t direction, uint32_t mode); 
void Stepper1_stop(void);
void Stepper2_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4);
void Stepper2_setSpeed(long whatSpeed);
void Stepper2_step(uint32_t steps, uint32_t direction, uint32_t mode); 
void Stepper2_stop(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __EC_STEPPER2_H
