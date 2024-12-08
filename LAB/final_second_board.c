#include "ecSTM32F4v2.h"

typedef struct _InputCapture
{
	uint32_t ovf_cnt;
	uint32_t distance;
	uint32_t timeInterval;
	float time1;
	float time2;
	int flag;
} IC;

typedef struct _USER
{
	uint8_t point;
	char User_name[10];
} USER;

IC echo[3] = {{0, 0, 0, 0.0, 0.0, 0},
			  {0, 0, 0, 0.0, 0.0, 0},
			  {0, 0, 0, 0.0, 0.0, 0}};

// USER parameter
USER user[10] = {
	{1, {"Kim"}},
	{1, {"Han"}},
	{1, {"Park"}},
	{1, {"MCU"}},
	{1, {"Bang"}},
	{1, {"Choi"}},
	{1, {"Sun"}},
	{1, {"Seo"}},
	{1, {"An"}},
	{1, {"Gong"}}};

#define TRIG PA_8
#define ECHO1 PB_6
#define ECHO2 PB_8
#define ECHO3 PB_10

void setup(void);

// Bluetooth parameter
uint8_t PC_Data = 0;
uint8_t BT_Data = 0;
uint8_t BT_string[40] = {0};
uint8_t start_string[15] = {"Loop\r\n\n\0"};
int BT_Flag = 0;
int Uart_On = 0;

// IC parameter
char recycle_name[3][10] = {{"Vinyl"}, {"Plastic"}, {"Can"}};

int sum[3] = {0};
int main(void)
{

	setup();
	USART6_write(start_string, 15);
	delay_ms(1000);
	Uart_On = 1;
	BT_Flag = 0;
	while (1)
	{
		USART6_write(start_string, 15);
		if (BT_Flag == 1)
		{
			// printf("get_data!\r\n");
			sprintf((char *)BT_string, "User %s score is a total of %d point\r\n\0", user[BT_Data].User_name, ++user[BT_Data].point);
			USART6_write(BT_string, 40);
			memset(BT_string, 0, 40);
			// printf("User %s score is a total of %d point\r\n", user[BT_Data].User_name, ++user[BT_Data].point);
			delay_ms(10000);
			// Recycling Bin Check
			for (int i = 0; i < 3; i++)
			{
				sum[0] += echo[0].distance;
				sum[1] += echo[1].distance;
				sum[2] += echo[2].distance;
				delay_ms(1000);
			}

			for (int i = 0; i < 3; i++)
			{
				if ((sum[i]) < 45)
				{
					sprintf(BT_string, "The %s Recycle bin is Full\r\n\0", recycle_name[i]);
					USART6_write(BT_string, 40);
					memset(BT_string, 0, 40);
					// printf("The %s Recycle bin is Full\r\n", recycle_name[i]);
				}
				sum[i] = 0;
			}
			BT_Flag = 0;
			Uart_On = 1;
		}
		delay_ms(5000);
	}
}

void setup()
{

	RCC_PLL_init();
	SysTick_init();
	UART2_init();

	// PWM configuration ---------------------------------------------------------------------
	PWM_init(TRIG);				 // PA_6: Ultrasonic trig pulse
	PWM_period_us(TRIG, 5000);	 // PWM of 50ms period. Use period_us()
	PWM_pulsewidth_us(TRIG, 10); // PWM pulse width of 10us

	// Input Capture configuration -----------------------------------------------------------------------
	ICAP_init(ECHO1);			   // PB_6 as input caputre
	ICAP_counter_us(ECHO1, 10);	   // ICAP counter step time as 10us
	ICAP_setup(ECHO1, 1, IC_RISE); // TIM4_CH1 as IC1 , rising edge detect
	ICAP_setup(ECHO1, 2, IC_FALL); // TIM4_CH2 as IC2 , falling edge detect
	ICAP_init(ECHO2);			   // PB_8 as input caputre
	ICAP_counter_us(ECHO2, 10);	   // ICAP counter step time as 10us
	ICAP_setup(ECHO2, 3, IC_RISE); // TIM4_CH3 as IC3 , rising edge detect
	ICAP_setup(ECHO2, 4, IC_FALL); // TIM4_CH4 as IC4 , falling edge detect
	ICAP_init(ECHO3);			   // PB_10 as input caputre
	ICAP_counter_us(ECHO3, 10);	   // ICAP counter step time as 10us
	ICAP_setup(ECHO3, 3, IC_RISE); // TIM2_CH3 as IC3 , rising edge detect
	ICAP_setup(ECHO3, 4, IC_FALL); // TIM2_CH4 as IC4 , falling edge detect

	// UART ---------------------------------------------------------------------
	// MCU
	UART1_init();
	UART1_baud(BAUD_9600);
	// Bluetooth
	UART6_init();
	UART6_baud(BAUD_9600);
}

void TIM4_IRQHandler(void)
{
	if (is_UIF(TIM4))
	{ // Update interrupt
		echo[0].ovf_cnt++;
		echo[1].ovf_cnt++;
		clear_UIF(TIM4); // clear update interrupt flag
	}

	if (is_CCIF(TIM4, 1))
	{ // TIM4_Ch1 (IC1) Capture Flag. Rising Edge Detect
		if (echo[0].flag)
		{										   // if Captured TimeEnd
			echo[0].time1 = ICAP_capture(TIM4, 1); // Capture TimeStart
			echo[0].flag = 0;
		}
		clear_CCIF(TIM4, 1); // clear capture/compare interrupt flag
	}
	else if (is_CCIF(TIM4, 2))
	{																								// TIM4_Ch2 (IC2) Capture Flag. Falling Edge Detect
		echo[0].time2 = ICAP_capture(TIM4, 2);														// Capture TimeEnd
		echo[0].timeInterval = (echo[0].time2 - echo[0].time1) + echo[0].ovf_cnt * (TIM4->ARR + 1); // (10us * counter pulse -> [msec] unit) Total time of echo1 pulse
		echo[0].distance = echo[0].timeInterval * 17 / 100;											// [mm] -> [cm]
		echo[0].ovf_cnt = 0;																		// overflow reset
		echo[0].flag = 1;																			// Capture TimeEnd flag
		clear_CCIF(TIM4, 2);																		// clear capture/compare interrupt flag
	}

	if (is_CCIF(TIM4, 3))
	{ // TIM4_Ch3 (IC3) Capture Flag. Rising Edge Detect
		if (echo[1].flag)
		{										   // if Captured TimeEnd
			echo[1].time1 = ICAP_capture(TIM4, 3); // Capture TimeStart
			echo[1].flag = 0;
		}
		clear_CCIF(TIM4, 3); // clear capture/compare interrupt flag
	}
	else if (is_CCIF(TIM4, 4))
	{																								// TIM4_Ch4 (IC4) Capture Flag. Falling Edge Detect
		echo[1].time2 = ICAP_capture(TIM4, 4);														// Capture TimeEnd
		echo[1].timeInterval = (echo[1].time2 - echo[1].time1) + echo[1].ovf_cnt * (TIM4->ARR + 1); // (10us * counter pulse -> [msec] unit) Total time of echo1 pulse
		if (echo[1].timeInterval > 50)
			echo[1].distance = echo[1].timeInterval * 17 / 100; // [mm] -> [cm]
		echo[1].ovf_cnt = 0;									// overflow reset
		echo[1].flag = 1;										// Capture TimeEnd flag
		clear_CCIF(TIM4, 4);									// clear capture/compare interrupt flag
	}
}

void TIM2_IRQHandler(void)
{
	if (is_UIF(TIM2))
	{					   // Update interrupt
		echo[2].ovf_cnt++; // overflow count
		clear_UIF(TIM2);   // clear update interrupt flag
	}
	if (is_CCIF(TIM2, 3))
	{ // TIM2_Ch1 (IC1) Capture Flag. Rising Edge Detect
		if (echo[2].flag)
		{										   // if Captured TimeEnd
			echo[2].time1 = ICAP_capture(TIM2, 3); // Capture TimeStart
			echo[2].flag = 0;
		}
		clear_CCIF(TIM2, 3); // clear capture/compare interrupt flag
	}
	else if (is_CCIF(TIM2, 4))
	{																								// TIM2_Ch2 (IC4) Capture Flag. Falling Edge Detect
		echo[2].time2 = ICAP_capture(TIM2, 4);														// Capture TimeEnd
		echo[2].timeInterval = (echo[2].time2 - echo[2].time1) + echo[2].ovf_cnt * (TIM2->ARR + 1); // (10us * counter pulse -> [msec] unit) Total time of echo1 pulse
		echo[2].distance = echo[2].timeInterval * 17 / 100;											// [mm] -> [cm]
		echo[2].ovf_cnt = 0;																		// overflow reset
		echo[2].flag = 1;																			// Capture TimeEnd flag
		clear_CCIF(TIM2, 4);																		// clear capture/compare interrupt flag
	}
}

/*******************  Bluetooth  ****************************/
void USART1_IRQHandler()
{ // USART2 RX Interrupt : Recommended
	if (Uart_On && is_USART1_RXNE())
	{
		BT_Data = USART1_read(); // RX from UART1 (BT)
		BT_Flag = 1;
		Uart_On = 0;
	}
}