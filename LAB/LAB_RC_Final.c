#include "ecSTM32F4v2.h"
#include "ecRCcar.h"

// ============================ Unit Definition ================================
MOTOR Motor = {
    {MOTOR_L, MOTOR_R}, // Pinnames
    {0.0, 0.0},         // Target duty
    {0.0, 0.0}          // duty
};

IC InputCap = {
    {TRIG, ECHO}, // PINs
    0,            // over_count
    0,            // time interval
    0,            // time start
    0,            // time end
    0             // flag
};

IR IRSensor = {
    {IR_L, IR_R}, // PINs
    {0, 0},       // IR value : left and right
    0             // IR_left - IR_right
};

// ======================= Parameter definition ========================
uint32_t state_speed = 0;
uint32_t state_Dir = S3;
uint32_t DIR_M = 0;   // 0 or 1; default = 0
uint32_t SPEED_M = 0; // 0 or 1; default = 0
uint8_t btData = 'S';
int DIR = 1; // Forward : 1, Backward : 0
float distance = 0;
int MODE = 0; // 0 = S, 1 = M, 2 = A
uint8_t BT_string[30] = {0};

// Function definition
void setup(void);
void ChangeState(void);
void AutoModeState(void);
void ImplementState(void);
void MotorRunning(void);
float duty_calcu(float target, int Dir);
void STOP(void);
void RisingDetect();
void FallingDetect();

// Main
int main()
{
   setup();
   while (1)
   {
      delay_ms(1000);
      if (MODE)
      {
         LED_toggle();
         sprintf((char *)BT_string, "MOD: A DIR: %d STR: %d VEL: %d\r\n", DIR, state_Dir, state_speed);
         USART1_write(BT_string, 30);
      }
      else
      {
         sprintf((char *)BT_string, "MOD: M DIR: %d STR: %d VEL: %d\r\n", DIR, state_Dir, state_speed);
         USART1_write(BT_string, 30);
      }
   }
}

// SETUP
void setup()
{
   RCC_PLL_init();
   SysTick_init();

   GPIO_init(LED_PIN, OUTPUT);

   // Motor Direction pins
   GPIO_init(DIR_PIN, OUTPUT);
   GPIO_write(DIR_PIN, DIR);

   // PWM Initiation
   PWM_init(MOTOR_L);
   PWM_init(MOTOR_R);
   PWM_period_us(MOTOR_L, 1000); // 1[ms]
   // btData = 'S';

   UART2_init();
   // Bluetooth
   UART1_init();
   UART1_baud(BAUD_9600);

   // Input Capture
   PWM_init(InputCap.PINS[0]);             // Triger Pin init; TIM 3
   PWM_period_us(InputCap.PINS[0], 50000); // 50[ms]
   PWM_pulsewidth_us(InputCap.PINS[0], 10);

   ICAP_init(InputCap.PINS[1]); // Echo Pin init
   ICAP_counter_us(InputCap.PINS[1], 100);
   ICAP_setup(InputCap.PINS[1], 1, IC_RISE);
   ICAP_setup(InputCap.PINS[1], 2, IC_FALL);

   // ADC pins
   JADC_init(IRSensor.PINs[LEFT]);
   JADC_init(IRSensor.PINs[RIGHT]);

   // ADC channel sequence setting
   JADC_sequence(IRSensor.PINs, 2);
}

// ===================================== Interrupt Handler ======================================

// Input Capture Handler (Priority 2)
void TIM4_IRQHandler(void)
{
   if (is_UIF(TIM4))
   { // Update interrupt
      if (InputCap.flag)
         InputCap.ovf_cnt++; // overflow count
      clear_UIF(TIM4);       // clear update interrupt flag
   }

   // TIM4_Ch1 (IC1) Capture Flag. Rising Edge Detect
   if (is_CCIF(TIM4, 1))
   {
      RisingDetect();
      clear_CCIF(TIM4, 1);
   }

   // TIM4_Ch2 (IC2) Capture Flag. Falling Edge Detect
   else if (is_CCIF(TIM4, 2))
   {
      FallingDetect();

      if (InputCap.TimeInterval < STOPDIST || btData == 'S')
         STOP();
      else
      {
         ImplementState();
         MotorRunning();
      }
      clear_CCIF(TIM4, 2);
   }
}

// Bluetooth Handler (Priority 1)
void USART1_IRQHandler()
{ // USART1 INT  Bluetooth
   if (is_USART_RXNE(USART1))
   {
      btData = USART_read(USART1);

      if (btData == 'M' || btData == 'm')
      {
         MODE = 0;
         GPIO_write(LED_PIN, 1);
      }
      else if (btData == 'A' || btData == 'a')
      {
         MODE = 1;
         state_speed = S3;
         DIR = 1;
      }
      if (btData == 'S' || btData == 's')
      {
         STOP();
      }
      else if (!MODE)
      {
         ChangeState();
         ImplementState();
         MotorRunning();
      }
      USART_write(USART1, &btData, 1);
      USART_write(USART1, (uint8_t *)"\r\n", 2);
   }
}
// ADC Interrupt (Priority 3)
void ADC_IRQHandler(void)
{
   if (is_ADC_OVR())
      clear_ADC_OVR();

   if (is_ADC_JEOC())
   { // after finishing sequence
      IRSensor.IR_value[LEFT] = JADC_read(1);
      IRSensor.IR_value[RIGHT] = JADC_read(2);
      IRSensor.IR_sub_L2R = IRSensor.IR_value[LEFT] - IRSensor.IR_value[RIGHT];
      if (MODE)
         AutoModeState();
      clear_ADC_JEOC();
   }
}

// ================================== Function ======================================
void ChangeState()
{
   switch ((char)btData)
   {
   case 'F':
      DIR = 1;
      GPIO_write(DIR_PIN, DIR);
      state_Dir = S3;
      break;
   case 'B':
      DIR = 0;
      GPIO_write(DIR_PIN, DIR);
      state_Dir = S3;
      break;
   case 'J':
      DIR_M = 0;
      state_Dir = FSM_Direction[state_Dir].Next[DIR_M];
      break;
   case 'L':
      DIR_M = 1;
      state_Dir = FSM_Direction[state_Dir].Next[DIR_M];
      break;
   case 'I':
      SPEED_M = 0;
      state_Dir = S3;
      state_speed = FSM_speed[state_speed].Next[SPEED_M];
      break;
   case 'K':
      SPEED_M = 1;
      state_Dir = S3;
      state_speed = FSM_speed[state_speed].Next[SPEED_M];
      break;
   default:
      break;
   }
}

void AutoModeState()
{
   // Adj direction
   if (IRSensor.IR_sub_L2R < -Limit_Max)
   { // Right side black hard = More right turn
      state_Dir = S6;
   }
   else if (IRSensor.IR_sub_L2R < -Limit_Mean)
   { // Left side black hard = More left turn
      state_Dir = S5;
   }
   else if (IRSensor.IR_sub_L2R < -Limit_Min)
   { // Left side black hard = More left turn
      state_Dir = S4;
   }
   else if (IRSensor.IR_sub_L2R > Limit_Max)
   { // Left side black hard = More left turn
      state_Dir = S0;
   }
   else if (IRSensor.IR_sub_L2R > Limit_Mean)
   { // Left side black hard = More left turn
      state_Dir = S1;
   }
   else if (IRSensor.IR_sub_L2R > Limit_Min)
   { // Left side black hard = More left turn
      state_Dir = S2;
   }
   else
   {
      state_Dir = S3; // Straight
   }
}

void ImplementState()
{
   Motor.TARGET[LEFT] = FSM_speed[state_speed].duty[LEFT] * FSM_Direction[state_Dir].duty_DIV[LEFT];
   Motor.TARGET[RIGHT] = FSM_speed[state_speed].duty[RIGHT] * FSM_Direction[state_Dir].duty_DIV[RIGHT];
   Motor.DUTY[LEFT] = duty_calcu(Motor.TARGET[LEFT], DIR);
   Motor.DUTY[RIGHT] = duty_calcu(Motor.TARGET[RIGHT], DIR);
}

void MotorRunning()
{
   PWM_duty(Motor.PINS[LEFT], Motor.DUTY[LEFT]);
   PWM_duty(Motor.PINS[RIGHT], Motor.DUTY[RIGHT]);
}

float duty_calcu(float target, int Dir)
{
   return fabs((float)Dir - target);
}

void STOP(void)
{
   float duty_stop = duty_calcu(0.0, DIR);
   PWM_duty(Motor.PINS[LEFT], duty_stop);
   PWM_duty(Motor.PINS[RIGHT], duty_stop);
}

void RisingDetect()
{
   InputCap.TimeStart = ICAP_capture(TIM4, 1);
   InputCap.flag = 1;
}

void FallingDetect()
{
   InputCap.TimeEnd = ICAP_capture(TIM4, 2);
   InputCap.TimeInterval = (uint32_t)((InputCap.TimeEnd - InputCap.TimeStart) + InputCap.ovf_cnt * (TIM4->ARR + 1)); // [10ms]
   InputCap.flag = 0;
   InputCap.ovf_cnt = 0;
}