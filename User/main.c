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

/* ���ڸ���λ���������� */
extern float Speed1Ref;
extern float Speed2Ref;
uint8_t Speed1RefTemp[4]; //�����ٶȴ洢����
uint8_t* Speed1RefPtr = (uint8_t*)&Speed1Ref; //ָ��
uint8_t Speed2RefTemp[4]; //�����ٶȴ洢����
uint8_t* Speed2RefPtr = (uint8_t*)&Speed2Ref; //ָ��
uint16_t checkCodeTemp; //У����洢

float RemoteForwardSpeed;//ͨ���������õ�ǰ���ٶ�
float RemoteRotateRadSpeed;//ͨ���������õ�ת���ٶ�
float HostForwardSpeed;//ͨ����λ�����õ�ǰ���ٶ�
float HostRotateRadSpeed;//ͨ����λ�����õ�ת���ٶ�

uint8_t State;//1ΪStop 2ΪForward 3ΪRotate

int main(void)
{
	/* ��ʼ�� */
	Timer_Init();
	OLED_Init();
	Motor_Init();
	LED_Init();
	Serial_Init();
	Encoder_Init();
	Host_Init();
	
	GreenLED_ON();//�������������У���PC13LED����

	/* OLED��ʾ��ʼ�� */
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

		// Serial_SendByte(0xA5);
		// checkCodeTemp = 0;
		// for(int i = 0; i < 4; i++)
		// {
		// 	Speed1RefTemp[i] = (Speed1RefPtr[i]&0xFF);
		// 	Speed2RefTemp[i] = (Speed2RefPtr[i]&0xFF);
		// 	checkCodeTemp += Speed1RefTemp[i] + Speed2RefTemp[i];
		// }
		// Serial_SendArray(Speed1RefTemp, 4);
		// Serial_SendArray(Speed2RefTemp, 4);
		// Serial_SendByte(checkCodeTemp&0xFF);
		// Serial_SendByte(0x5A);
	}
}

/* Get current time(ms)(max time is 1193 hours) */
void TIM2_IRQHandler(void)//����жϺ����������ļ�����//�����ٶȲ��ȶ����жϼ����Ϊ10ms
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		CurrentTime++;
		// CarSpeedSet(RemoteForwardSpeed,RemoteRotateRadSpeed);
		CarSpeedSet(HostForwardSpeed,HostRotateRadSpeed);
		Motor_Update();
 		if(CurrentTime%100 == 0)
 		{
			GreenLED_Turn();
 		}

		/* Timer debug */
		// OLED_ShowString(1, 1, "Time:");
		// OLED_ShowNum(1, 7, CurrentTime, 9);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

