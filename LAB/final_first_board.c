#define _CRT_SECURE_NO_WARNINGS
#include "stm32f411xe.h"
#include "ecPinNames.h"
#include "ecGPIO2.h"
#include "ecRCC2.h"
#include "ecTIM2.h"
#include "ecPWM2.h"
#include "ecSysTick2.h"
#include "ecICAP2.h"
#include "ecADC2.h"
#include "ecUART2.h"
#include "ecStepper.h"
#include <string.h>
#include <stdio.h>


#define RC1_PIN    PA_0
#define TX_PIN     PA_9
#define RX_PIN     PA_10
#define LED_R      PA_5
#define LED_Y      PA_6
#define LED_G      PA_7
#define Bar_TX     PA_11
#define Bar_RX		 PA_12
#define IR_PIN     PB_0



// USART variables
char barcode[14]={0};
int bar_counter=0;
int Barcode_flag=0;
static volatile uint8_t barcode_data = 0;


//Barcode variables

typedef struct
{
	uint8_t classfication;
	char product_bar[14];
} recycle;

recycle recycle_data[30] = {

	{2, {"8801097481206"}}, //pet
	{1, {"8804973307546"}}, //vinyl
	{1, {"8801062862863"}}, //vinyl
	{3, {"8801056154011"}}, //can
	{2, {"8801056128818"}}, //pet 
	{2, {"8801056051150"}}, //pet
	{1, {"8801037095876"}}, //vinyl
	{2, {"8801094132705"}}, //pet
	{3, {"8801043450690"}}, //can
	{3, {"8801097160064"}}, //can
	{3, {"8801056038861"}}, //can
	{2, {"8801056049881"}}, //pet
	{2, {"8801056042134"}}, //pet
	{2 ,{"8801094362706"}}  //pet
};

typedef struct
{
	uint16_t usernum;
	char user_bar[13];
} user;

user user_data[10] = {
	{0, {"2200018802193"}}, 
	{1, {"2190003101052"}},
	{2, {"2180027504076"}},
	{3, {"2210060008252"}},
	{4, {"2210033309010"}},
	{5, {"2210072711119"}},
	{6, {"2200009012280"}},
	{7, {"2210033302097"}},
	{8, {"1234567891231"}},
	{9, {"0209012036216"}}
};

static volatile uint8_t user_num=0;
int product_num=0;
int reset_counter=0;

//IR 

int IR_val=0;
int IR_counter=0;
int IR_flag=0;

//system flag
int user_flag=0;
int product_flag=0;
int end_flag=0;
int stepper_ready=0;

int error_flag=0;

 


//function
void setup(void);
void Barcode_detector(void);
void step_motor_control(void);
void LED_control(void);
void RC_motor_control(void);
void hand_detector(void);


int main(void) {
	setup();
	printf("MCU Initialized\r\n");

  while (1) {
		
		}
}

// Initial setup
void setup(void) {
	RCC_PLL_init();                      // System Clock = 84MHz
	SysTick_init();                      // Systick init
	
	// LED Configureation -------------------------------------------------------------------------------
	GPIO_init(LED_R,OUTPUT);
	GPIO_otype(LED_R, PUSH_PULL);
	GPIO_pupd(LED_R,NPUPD);
	GPIO_ospeed(LED_R,MS);
	
	GPIO_init(LED_Y,OUTPUT);
	GPIO_otype(LED_Y, PUSH_PULL);
	GPIO_pupd(LED_Y,NPUPD);
	GPIO_ospeed(LED_Y,MS);
	
	GPIO_init(LED_G,OUTPUT);
	GPIO_otype(LED_G, PUSH_PULL);
	GPIO_pupd(LED_G,NPUPD);
	GPIO_ospeed(LED_G,MS);
	
	
	
	
	// RC Motor PWM configuration -----------------------------------------------------------------------	
	PWM_init(RC1_PIN);
	PWM_period_ms(RC1_PIN, 20);
	PWM_duty(RC1_PIN,0.025);

	// Step Motor configuration -------------------------------------------------------------------------
	Stepper1_init(PB_10,PB_4,PB_5,PB_3);
	Stepper2_init(PB_2,PB_1,PB_15,PB_14);
	
	Stepper1_setSpeed(3);
	Stepper2_setSpeed(3);
	
	// IR configuration ---------------------------------------------------------------------	
	ADC_init(IR_PIN);
	
	
	// USART configuration(for test)-------------------------------------------------------------------
	UART2_init();
	UART2_baud(BAUD_115200);

	// USART1: board connect --------------------------------------------------------------------------- 
	UART1_init();
	UART1_baud(BAUD_9600);
	
	
	// USART6: barcode RX,TX-------------------------------------------------------------------------
	UART6_init();
	UART6_baud(BAUD_115200);
	
	TIM_UI_init(TIM2,10);
	
}

void TIM2_IRQHandler(void){
	if(is_UIF(TIM2)){  // Update interrupt

		Barcode_detector();
		step_motor_control();
		LED_control();
		RC_motor_control();
		
		if(end_flag==1) USART1_write(&user_num,1);
		product_flag = 0;
		

		clear_UIF(TIM2);  							    // clear update interrupt flag
	}
}




void Barcode_detector(void){

	
	if (Barcode_flag==1 && user_flag==0){
		for(int i=0 ; i<10 ; i++){
			if (!strcmp(user_data[i].user_bar,barcode)){
				user_flag=1;
				user_num=user_data[i].usernum;
			}
			bar_counter=0;
			Barcode_flag=0;
			reset_counter=0;
		}

	}
	if(Barcode_flag==1 && user_flag==1){
		for(int i=0 ; i<30 ; i++){
			if (!strcmp(recycle_data[i].product_bar,barcode)){
				product_flag=1;
				product_num=recycle_data[i].classfication;
				user_flag=0;
			}
		}
	  bar_counter=0;
		Barcode_flag=0;
		reset_counter=0;
	}

	
	
}

void step_motor_control(void){
	int dir=1;
	int step=0;
	if (stepper_ready==1){
		dir=1;
	  step=0;
	}
	if (product_flag==1) {
		dir=0;
		step=800;
		stepper_ready=1;
	}

	if(end_flag==1) {
		dir=1;
		step=800;
		end_flag = 0;
	}
	if(product_num==1){	
		Stepper1_step(step,dir,HALF);
	}
	if(product_num==2){
		Stepper2_step(step,dir,HALF);
	}
}

void LED_control(void){
	if(user_flag==0&&product_flag==0) {
		GPIO_write(LED_Y,0);
		GPIO_write(LED_G,0);
		GPIO_write(LED_R,1);
	}
	
	if(user_flag==1) {
		GPIO_write(LED_Y,1);
		GPIO_write(LED_R,0);
//		printf("Y");
	}
	
	if(stepper_ready==1) {
		GPIO_write(LED_G,1); 
		GPIO_write(LED_Y,0);
		GPIO_write(LED_R,0);
//		printf("G");
	}

}


void RC_motor_control(void){
	
	if(stepper_ready==1) PWM_duty(RC1_PIN,0.125);
	hand_detector();
	if(end_flag==1) PWM_duty(RC1_PIN,0.025);
}

void ADC_IRQHandler(void) {
    if (is_ADC_OVR())
        clear_ADC_OVR();

   if(is_ADC_EOC()){      // after finishing sequence
      IR_val = ADC_read(); 
	 }		 

}


void hand_detector(void){
	
	if(stepper_ready==1){
		if(IR_val<3800){
			IR_flag=1;
		}
	}
	
	if(IR_flag==1){
		IR_counter++;
	}
	
	if(IR_counter>600) {
		IR_counter=0;
		IR_flag=0;
		end_flag=1;
		stepper_ready=0;
	}
}



void USART6_IRQHandler(){          		// USART2 RX Interrupt : Recommended
	if(is_USART6_RXNE()){
		barcode_data= USART6_read();		// RX from UART1 (BT)	
		barcode[bar_counter]=barcode_data;
		bar_counter++;
		if(bar_counter>13) {
			barcode[bar_counter - 1] = 0;
			Barcode_flag=1;
			bar_counter=0;
		}
	}
	
}