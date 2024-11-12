#ifndef __EC_RCcar_H
#define __EC_RCcar_H

#include "ecSTM32F4v2.h"

// Macro Definition
#define V_MIN  		0.4
#define V_MEAN 		0.6
#define V_HIGH 		0.8
#define V_FAST 		1.0
#define DIV_4 		0.3
#define DIV_3 		0.4
#define DIV_2 		0.6
#define DIV_1 		1.0
#define STOPDIST  8.0
// TIM2 CH1
#define MOTOR_R 	PA_1
// TIM2 CH2
#define MOTOR_L 	PA_0
#define DIR_PIN 	PC_2
// TIM 1
#define TRIG      PA_8
// TIM 4
#define ECHO      PB_6
#define IR_L 			PB_1
#define IR_R			PB_0

// ===================================== Enum definition =======================================
typedef enum{
	Limit_Max  = 2000,		// S0, S6
	Limit_Mean = 1500,		// S1, S5
	Limit_Min  = 700			// S2, S4
} LIMIT;

typedef enum{
   S0,
   S1,
   S2,
   S3,
   S4,
   S5,
   S6
} STATE;

typedef enum{
   LEFT,
   RIGHT
} Motor_Param;

typedef struct{
   PinName_t PINS [2]; 
   float TARGET [2];
   float DUTY [2];
} MOTOR;

typedef struct{
   PinName_t PINS [2];
   uint32_t    ovf_cnt;
   uint32_t TimeInterval;
   float TimeStart;
   float TimeEnd;
   uint8_t flag;
} IC;

typedef struct{
	PinName_t PINs [2];
	uint32_t IR_value [2];
	int32_t	IR_sub_L2R;	
} IR;

// =================================================== Structure Definition =================================================== 
typedef struct {
   float duty[2];
   uint32_t Next[2];
}Motor_speed;

typedef struct {
   float duty_DIV[2];
   uint32_t Next[2];
}Motor_Direction;

// FSM Definition
Motor_speed FSM_speed[4] = {
   {{V_MIN, V_MIN}, {S1, S0}},		// S0
   {{V_MEAN, V_MEAN}, {S2, S0}},	// S1
   {{V_HIGH, V_HIGH}, {S3, S1}},	// S2
   {{V_FAST, V_FAST}, {S3, S2}} 	// S3
};

// LEFT, RIGHT
Motor_Direction FSM_Direction[7] = {
   {{DIV_4, DIV_1}, {S0, S1}},		// S0
   {{DIV_3, DIV_1}, {S0, S2}},		// S1
   {{DIV_2, DIV_1}, {S1, S3}},		// S2
   {{DIV_1, DIV_1}, {S2, S4}},		// S3
   {{DIV_1, DIV_2}, {S3, S5}},		// S4
   {{DIV_1, DIV_3}, {S4, S6}},		// S5
   {{DIV_1, DIV_4}, {S5, S6}}			// S6
};

#endif // __EC_RCcar_H
