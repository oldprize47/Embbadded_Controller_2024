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
volatile Stepper_t myStepper;
PinName_t Stepper_pins[4] = {0};

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

void Stepper_init(PinName_t pinName1, PinName_t pinName2, PinName_t pinName3, PinName_t pinName4)
{

	//  GPIO Digital Out Initiation
	myStepper.pin1 = pinName1;
	myStepper.pin2 = pinName2;
	myStepper.pin3 = pinName3;
	myStepper.pin4 = pinName4;

	Stepper_pins[0] = myStepper.pin1;
	Stepper_pins[1] = myStepper.pin2;
	Stepper_pins[2] = myStepper.pin3;
	Stepper_pins[3] = myStepper.pin4;
	//  GPIO Digital Out Initiation
	// No pull-up Pull-down , Push-Pull, Fast
	for (int i = 0; i < 4; i++)
	{
		GPIO_init(Stepper_pins[i], OUTPUT);
		GPIO_pupd(Stepper_pins[i], NOPUPD);
		GPIO_otype(Stepper_pins[i], PUSHPULL);
		GPIO_ospeed(Stepper_pins[i], FAST_SPEED);
	}
}

void Stepper_pinOut(uint32_t state, uint32_t mode)
{
	if (mode == FULL)
	{ // FULL mode
		for (int i = 3; i >= 0; i--)
			GPIO_write(Stepper_pins[3 - i], (FSM_full[state].out >> i) & 1);
	}
	else if (mode == HALF)
	{ // HALF mode
		for (int i = 3; i >= 0; i--)
			GPIO_write(Stepper_pins[3 - i], (FSM_full[state].out >> i) & 1);
	}
}

void Stepper_setSpeed(long whatSpeed)					// rpm [rev/min]
{													   
	step_delay = 60.0 * 1000.0 / whatSpeed / step_per_rev; 	// Convert rpm to  [msec/step] delayW
}

void Stepper_step(uint32_t steps, uint32_t direction, uint32_t mode)
{
	uint32_t state = 0;
	myStepper._step_num = steps;

	for (; myStepper._step_num > 0; myStepper._step_num--)
	{						  // run for step size
		delay_ms(step_delay); // delay (step_delay);
		if (mode == FULL)
			state = FSM_full[state].next[direction]; // state = next state
		else if (mode == HALF)
			state = FSM_half[state].next[direction]; // state = next state
		Stepper_pinOut(state, mode);
	}
}

void Stepper_stop(void)
{
	myStepper._step_num = 0;
	// All pins(A,AN,B,BN) set as DigitalOut '0'
	for (int i = 0; i < 4; i++)
		GPIO_write(Stepper_pins[i], 0);
	Stepper_setSpeed(0);
}