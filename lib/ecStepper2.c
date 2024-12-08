#include "stm32f4xx.h"
#include "ecStepper2.h"

// State number
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4
#define S5 5
#define S6 6
#define S7 7

// Stepper Motor function
uint32_t direction = 1;
uint32_t step_delay = 100;
uint32_t step_per_rev = 64 * 32;

// Stepper Motor variable
volatile Stepper_t myStepper1;
volatile Stepper_t myStepper2;

// FULL stepping sequence  - FSM
typedef struct
{
	uint32_t next[2];
	uint8_t out;
} State_full_t;

// 0b A B A' B'
// X = 0 DIR = 0, X=1 DIR = 1
State_full_t FSM_full[4] = {
	// 1100 , 0110 , 0011 , 1001

	{{S1, S3}, 0b1100}, // ABA'B'
	{{S2, S0}, 0b0110}, // ABA'B'
	{{S3, S1}, 0b0011}, // ABA'B'
	{{S0, S2}, 0b1001}, // ABA'B'
};

// HALF stepping sequence
typedef struct
{
	uint32_t next[2];
	uint8_t out;
} State_half_t;

State_half_t FSM_half[8] = {
	// 1000 , 1100 , 0100 , 0110 , 0010 , 0011, 0001, 1001
	{{S1, S7}, 0b1000}, // ABA'B'
	{{S2, S0}, 0b1100},
	{{S3, S1}, 0b0100},
	{{S4, S2}, 0b0110},
	{{S5, S3}, 0b0010},
	{{S6, S4}, 0b0011},
	{{S7, S5}, 0b0001},
	{{S0, S6}, 0b1001},
};

void Stepper1_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4)
{

	//  GPIO Digital Out Initiation
	myStepper1.pin[0] = pinName1;
	myStepper1.pin[1] = pinName2;
	myStepper1.pin[2] = pinName3;
	myStepper1.pin[3] = pinName4;
	//  GPIO Digital Out Initiation
	// No pull-up Pull-down , Push-Pull, Fast
	for (int i = 0; i < 4; i++)
	{
		GPIO_init(myStepper1.pin[i], OUTPUT);
		GPIO_pupd(myStepper1.pin[i], NOPUPD);
		GPIO_otype(myStepper1.pin[i], PUSHPULL);
		GPIO_ospeed(myStepper1.pin[i], FAST_SPEED);
	}
}

void Stepper1_pinOut(uint32_t state, uint32_t mode)
{
	if (mode == FULL)
	{ // FULL mode
		for (int i = 3; i >= 0; i--)
			GPIO_write(myStepper1.pin[3 - i], (FSM_full[state].out >> i) & 1);
	}
	else if (mode == HALF)
	{ // HALF mode
		for (int i = 3; i >= 0; i--)
			GPIO_write(myStepper1.pin[3 - i], (FSM_full[state].out >> i) & 1);
	}
}

void Stepper1_setSpeed(long whatSpeed)					// rpm [rev/min]
{													   
	step_delay = 60.0 * 1000.0 / whatSpeed / step_per_rev; 	// Convert rpm to  [msec/step] delayW
}

void Stepper1_step(uint32_t steps, uint32_t direction, uint32_t mode)
{
	uint32_t state = 0;
	myStepper1._step_num = steps;

	for (; myStepper1._step_num > 0; myStepper1._step_num--)
	{						  // run for step size
		delay_ms(step_delay); // delay (step_delay);
		if (mode == FULL)
			state = FSM_full[state].next[direction]; // state = next state
		else if (mode == HALF)
			state = FSM_half[state].next[direction]; // state = next state
		Stepper1_pinOut(state, mode);
	}
}

void Stepper1_stop(void)
{
	myStepper1._step_num = 0;
	// All pins(A,AN,B,BN) set as DigitalOut '0'
	for (int i = 0; i < 4; i++)
		GPIO_write(myStepper1.pin[i], 0);
	Stepper1_setSpeed(0);
}

void Stepper2_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4)
{

	//  GPIO Digital Out Initiation
	myStepper2.pin[0] = pinName1;
	myStepper2.pin[1] = pinName2;
	myStepper2.pin[2] = pinName3;
	myStepper2.pin[3] = pinName4;

	//  GPIO Digital Out Initiation
	// No pull-up Pull-down , Push-Pull, Fast
	for (int i = 0; i < 4; i++)
	{
		GPIO_init(myStepper2.pin[i], OUTPUT);
		GPIO_pupd(myStepper2.pin[i], NOPUPD);
		GPIO_otype(myStepper2.pin[i], PUSHPULL);
		GPIO_ospeed(myStepper2.pin[i], FAST_SPEED);
	}
}

void Stepper2_pinOut(uint32_t state, uint32_t mode)
{
	if (mode == FULL)
	{ // FULL mode
		for (int i = 3; i >= 0; i--)
			GPIO_write(myStepper2.pin[3 - i], (FSM_full[state].out >> i) & 1);
	}
	else if (mode == HALF)
	{ // HALF mode
		for (int i = 3; i >= 0; i--)
			GPIO_write(myStepper2.pin[3 - i], (FSM_full[state].out >> i) & 1);
	}
}

void Stepper2_setSpeed(long whatSpeed)					// rpm [rev/min]
{													   
	step_delay = 60.0 * 1000.0 / whatSpeed / step_per_rev; 	// Convert rpm to  [msec/step] delayW
}

void Stepper2_step(uint32_t steps, uint32_t direction, uint32_t mode)
{
	uint32_t state = 0;
	myStepper2._step_num = steps;

	for (; myStepper2._step_num > 0; myStepper2._step_num--)
	{						  // run for step size
		delay_ms(step_delay); // delay (step_delay);
		if (mode == FULL)
			state = FSM_full[state].next[direction]; // state = next state
		else if (mode == HALF)
			state = FSM_half[state].next[direction]; // state = next state
		Stepper2_pinOut(state, mode);
	}
}

void Stepper2_stop(void)
{
	myStepper2._step_num = 0;
	// All pins(A,AN,B,BN) set as DigitalOut '0'
	for (int i = 0; i < 4; i++)
		GPIO_write(myStepper2.pin[i], 0);
	Stepper2_setSpeed(0);
}