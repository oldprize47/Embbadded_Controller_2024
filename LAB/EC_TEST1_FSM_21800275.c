/*******************************************************
		Name: Sangheon			ID: 21800275
		Date: 2024-10-15
		TEST 1
********************************************************/
#include "ecSTM32F4v2.h"
#include <stdlib.h>

// State definition
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4
#define S5 5

// Address number of output in array
#define DIR 0
#define PWM 1
#define SEGMENT 2
#define BLINK 3

typedef struct
{
	uint32_t next[2]; // nextstate = FSM[state].next[input]
	double out[4];	  // output = FSM[state].out[PWM or LED]
} State_t;

State_t FSM[6] = {
	{{S0, S1}, {0, 0, 0, 0}},
	{{S1, S2}, {0, 0.55, 1, 1}},
	{{S2, S3}, {0, 1, 1, 0}},
	{{S3, S4}, {0, 0, 0, 0}},
	{{S4, S5}, {1, 0.55, -1, 1}},
	{{S5, S0}, {1, 1, -1, 0}},
};
#define BUTTON_PIN PC_13
#define DIR_PIN PC_2
#define PWM_PIN PA_0
#define IR_PIN PB_1

unsigned char state = S0; // FSM state
unsigned char input = 0;  // Button flag
double pwm_power = 0.0;	  // 0 ~ 1
double duty = 0;		  // 0 ~ 1
int num_add = 0;		  // 1 or -1
int seg_num = 0;		  // 7 segment num
int dir = 0;			  // DC Motor DIR
unsigned int blink = 0;	  // for 1sec
unsigned int rate = 0;	  // 7 segment Count Control
int IR_flag = 0;

void setup(void);
void nextState();
void stateOutput();

int main(void)
{

	setup();
	// Inifinite Loop ----------------------------------------------------------
	while (1)
	{
	}
}

void TIM3_IRQHandler(void)
{
	if (is_UIF(TIM3))
	{ // update interrupt flag
		// First, Update next state. Then, Output.  Repeat
		// 1. Update State <-- Next State
		nextState();

		// 2. Output of states
		stateOutput();

		// DC Motor
		duty = fabs(dir - pwm_power);
		PWM_duty(PWM_PIN, duty);

		// 7 segment
		if (rate > blink)
		{
			seg_num += num_add;
			rate = 0;
		}
		if (seg_num > 9)
			seg_num = 0;
		else if (seg_num < 0)
			seg_num = 9;
		if (state % 3 != 0)
			sevensegment_display(seg_num % 10);
		else
			sevensegment_Off();

		clear_UIF(TIM3); // clear by writing 0
	}
}
void EXTI1_IRQHandler(void)
{
	if (is_pending_EXTI(IR_PIN))
	{
		state = S0;
		seg_num = 0;
		stateOutput();
		clear_pending_EXTI(IR_PIN);
		input = 0;
	}
}

void EXTI15_10_IRQHandler(void)
{

	if (is_pending_EXTI(BUTTON_PIN))
	{
		input = 1;
		clear_pending_EXTI(BUTTON_PIN);
	}
}

void nextState()
{
	IR_flag = GPIO_read(IR_PIN);
	if (input == 1 && IR_flag)
	{
		state = FSM[state].next[input];
		delay_ms(40);
	}

	input = 0;
}

void stateOutput()
{
	dir = FSM[state].out[DIR];
	GPIO_write(DIR_PIN, dir);
	pwm_power = FSM[state].out[PWM];
	blink = FSM[state].out[BLINK];
	num_add = FSM[state].out[SEGMENT];
	rate++;
}

// Initialiization
void setup(void)
{
	// Initialiization --------------------------------------------------------
	RCC_PLL_init(); // System Clock = 84MHz
	SysTick_init();

	// BUTTON_PIN init
	GPIO_init(BUTTON_PIN, INPUT);
	GPIO_pupd(BUTTON_PIN, PULL_DOWN);

	// IR_PIN init
	GPIO_init(IR_PIN, INPUT);
	GPIO_pupd(IR_PIN, PULL_DOWN);

	// Sevensegment_PIN init
	sevensegment_display_init(PA_7, PB_6, PC_7, PA_9); // Decoder input A, B, C, D for Problem 1

	// Direction Pin init
	GPIO_init(DIR_PIN, OUTPUT);
	GPIO_otype(DIR_PIN, PUSHPULL);
	GPIO_write(DIR_PIN, dir);

	// PWM_PIN init
	PWM_init(PWM_PIN);		// GPIOA 0 ALTERNATE function
	PWM_period(PWM_PIN, 1); // msec = 1 (Only range 1 to 655 is allowed)

	// Timer init
	TIM_UI_init(TIM3, 500); // msec = 500 (Only range 1 to 655 is allowed)

	// EXTI_BUTTON_PIN
	EXTI_init(BUTTON_PIN, RISE, 0); // Pin Type, Trig Type(Falling, Rising, Both), Priority
	EXTI_enable(BUTTON_PIN);

	// EXTI_IR_PIN
	EXTI_init(IR_PIN, FALL, 0); // Pin Type, Trig Type(Falling, Rising, Both), Priority
	EXTI_enable(IR_PIN);
}