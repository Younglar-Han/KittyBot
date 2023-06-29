#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "LED.h"
#include "Serial.h"
#include "Encoder.h"
#include "Timer.h"
#include "PID.h"
#include "Serial_OpenMV.h"

uint32_t CurrentTime;
bool ArriveFlag = false;
bool RotateFlag = false;
extern float Position1;
extern float Position2;

uint8_t State;//1为Stop 2为Forward 3为Rotate

int main(void)
{
	/* 初始化 */
	Timer_Init();
	OLED_Init();
	Motor_Init();
	LED_Init();
	Serial_Init();
	Encoder_Init();
	OpenMV_Init();
	
	RedLED_ON();//若程序正常运行，则PC13LED亮起

	/* OLED显示初始化 */
//	OLED_ShowString(1, 1, "----Younglar----");
	// OLED_ShowString(2, 1, "Left:");
	// OLED_ShowString(3, 1, "Right:");
	// OLED_ShowString(4, 1, "State:Stop");

	while(1)
	{
				/* Encoder debug */
		OLED_ShowString(1, 1, "Ecd1:");
		OLED_ShowSignedNum(1, 8, Ecd1_Get(), 7);
		OLED_ShowString(2, 1, "Ecd2:");
		OLED_ShowSignedNum(2, 8, Ecd2_Get(), 7);
		

		char str1[10];
		sprintf(str1, "%.6lf", Speed1_Get());
		OLED_ShowString(3, 1, "Speed1:");
		OLED_ShowString(3, 9, str1);
		
		char str2[10];
		sprintf(str2, "%.6lf", Speed2_Get());
		OLED_ShowString(4, 1, "Speed2:");
		OLED_ShowString(4, 9, str2);
	}
}

/* Get current time(ms)(max time is 1193 hours) */
void TIM2_IRQHandler(void)//这个中断函数在启动文件里找//由于速度不稳定，中断间隔改为10ms
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		CurrentTime++;
		Motor_Update();
 		if(CurrentTime%100 == 0)
 		{
			RedLED_Turn();
 		}

		/* Timer debug */
		// OLED_ShowString(1, 1, "Time:");
		// OLED_ShowNum(1, 7, CurrentTime, 9);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

