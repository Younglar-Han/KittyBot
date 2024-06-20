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
#include "Serial_Host.h"

uint32_t CurrentTime;
bool ArriveFlag = false;
bool RotateFlag = false;
extern float Position1;
extern float Position2;

/* 用于给上位机反馈数据 */
extern float Speed1Ref;
extern float Speed2Ref;
uint8_t Speed1RefTemp[4]; //左轮速度存储数组
uint8_t* Speed1RefPtr = (uint8_t*)&Speed1Ref; //指针
uint8_t Speed2RefTemp[4]; //右轮速度存储数组
uint8_t* Speed2RefPtr = (uint8_t*)&Speed2Ref; //指针
extern float LinearXRef;
extern float AngularZRef;
uint8_t LinearXRefTemp[4]; //线速度存储数组
uint8_t* LinearXRefPtr = (uint8_t*)&LinearXRef; //指针
uint8_t AngularZRefTemp[4]; //角速度存储数组
uint8_t* AngularZRefPtr = (uint8_t*)&AngularZRef; //指针
uint16_t checkCodeTemp; //校验码存储

float RemoteForwardSpeed;//通过蓝牙设置的前进速度
float RemoteRotateRadSpeed;//通过蓝牙设置的转向速度
uint8_t mode;//0：上位机模式 1：蓝牙模式
float HostForwardSpeed;//通过上位机设置的前进速度
float HostRotateRadSpeed;//通过上位机设置的转向速度

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
	Host_Init();
	
	GreenLED_ON();//若程序正常运行，则PC13LED亮起

	/* OLED显示初始化 */
	// OLED_ShowString(1, 1, "----Younglar----");
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
		/* Timer debug */
		// OLED_ShowString(1, 1, "Time:");
		// OLED_ShowNum(1, 7, CurrentTime, 9);
		ToHost_SendByte(0xA5);
		checkCodeTemp = 0;
		for(int i = 0; i < 4; i++)
		{
			LinearXRefTemp[i] = (LinearXRefPtr[i]&0xFF);
			AngularZRefTemp[i] = (AngularZRefPtr[i]&0xFF);
			checkCodeTemp += LinearXRefTemp[i] + AngularZRefTemp[i];
		}
		ToHost_SendArray(LinearXRefTemp, 4);
		ToHost_SendArray(AngularZRefTemp, 4);
		ToHost_SendByte(checkCodeTemp&0xFF);
		ToHost_SendByte(0x5A);
	}
}

/* Get current time(ms)(max time is 1193 hours) */
void TIM2_IRQHandler(void)//这个中断函数在启动文件里找//由于速度不稳定，中断间隔改为10ms
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		CurrentTime++;
		if(mode)
			CarSpeedSet(RemoteForwardSpeed,RemoteRotateRadSpeed);
		else
			CarSpeedSet(HostForwardSpeed,HostRotateRadSpeed);
		Motor_Update();
 		if(CurrentTime%100 == 0)
 		{
			GreenLED_Turn();
 		}

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

