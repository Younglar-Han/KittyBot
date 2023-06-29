#include "stm32f10x.h"                  // Device header

#define Pi 3.14159265358979

int32_t Encoder1_Count;//这里改成int32后大大提高幅值，减少溢出
int32_t Encoder2_Count;//但是溢出的情况没有考虑

float Angle1;
float Angle2;

float Position1;
float Position2;

void Encoder_Init(void)
{
	/* Enable RCC Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/* GPIO Init */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Enable AFIO */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);

	/* EXTI Init */
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line6 | EXTI_Line7;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发中断
	EXTI_Init(&EXTI_InitStructure);
	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//整个芯片只能用同一种分组方式，只在这里定义一次
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);	
}

int32_t Ecd1_Get(void)
{
	return Encoder1_Count;
}
int32_t Ecd2_Get(void)
{
	return Encoder2_Count;
}

/* 获取电机1 已经 转过的总角度(rad)（130脉冲对应半圈）*/
float Angle1_Get(void)
{
	Angle1 = Encoder1_Count / 130.0f * Pi;
	return Angle1;
}

/* 获取轮1 已经 转过的总距离(m)（轮半径为23.5mm）*/
float Position1_Get(void)
{
	Angle1_Get();
	Position1 = Angle1*23.5f/1000;
	return Position1;
}

/* 获取电机2 已经 转过的总角度(rad)（130脉冲对应半圈）*/
float Angle2_Get(void)
{
	Angle2 = Encoder2_Count / 130.0f * Pi;
	return Angle2;
}

/* 获取轮2 已经 转过的总距离(m)（轮半径为23.5mm）*/
float Position2_Get(void)
{
	Angle2_Get();
	Position2 = Angle2*23.5f/1000;
	return Position2;
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) == SET)//这里对于01234这种一个pin对应一个EXTI通道的情况其实不用判断
	{	
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
		{
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
			{
				Encoder1_Count ++;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) == SET)//接上文，只有多pin对应一通道的情况要判断
	{
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
		{
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
			{
				Encoder1_Count --;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6) == SET)//现在的判断不是多余的
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0)
		{
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0)
			{
				Encoder2_Count --;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	if(EXTI_GetITStatus(EXTI_Line7) == SET)
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == 0)
		{
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0)
			{
				Encoder2_Count ++;
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
}
