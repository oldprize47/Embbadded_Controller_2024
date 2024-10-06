/**
******************************************************************************
* @course   Embedded Controller- HGU
* @author	SangheonPark
* @mod		2024-9-27 by PSH
* @brief	STM32F411 Library for EC
*
******************************************************************************
*/

#include "ecSTM32F4v2.h"

void MCU_init(void){
    // CLOCK PLL 84MHz
    RCC_PLL_init();
    
    // SysTick 1msec
//    SysTick_init();    
    
    // Button PC13
    GPIO_init(PC_13, INPUT);
    GPIO_pupd(PC_13, PULL_DOWN);    
    
    // LED PA5
    GPIO_init(PA_5, OUTPUT);    

    // TIMx Default Initialization
    // ...
    // PWM Default Initialization
    // ...        
    // USART Default Initialization
    // ...
    // Others
}