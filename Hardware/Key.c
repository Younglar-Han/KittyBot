#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)//PA4 PB2
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t Key_Num = 0;
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 1)
	{
		// Delay_ms(20);
		// while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 1);
		// Delay_ms(20);
		Key_Num = 1;
	}
	return Key_Num;
}

uint8_t Get_HasDrag(void)
{
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 1)
	{
		return 1;
	}else
	{
		return 0;
	}
}

/* 中断实现按键功能 */
// void Key_Init(void)
// {
// 	/* Enable RCC Clock */
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
// 	/* GPIO Init */
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);

// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
// 	/* Enable AFIO */
// 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource2);
// 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);

// 	/* EXTI Init */
// 	EXTI_InitTypeDef EXTI_InitStructure;
// 	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line4;
// 	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
// 	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
// 	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发中断
// 	EXTI_Init(&EXTI_InitStructure);
	

// 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//整个芯片只能用同一种分组方式，只在这里定义一次
	
// 	NVIC_InitTypeDef NVIC_InitStructure;
// 	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
// 	NVIC_Init(&NVIC_InitStructure);
	
// 	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
// 	NVIC_Init(&NVIC_InitStructure);
// }


// void EXTI2_IRQHandler(void)
// {
// 	if(EXTI_GetITStatus(EXTI_Line2) == SET)//这里对于01234这种一个pin对应一个EXTI通道的情况其实不用判断
// 	{	
// 		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == 1)
// 		{
			
// 		}
// 		EXTI_ClearITPendingBit(EXTI_Line2);
// 	}
// }

// void EXTI4_IRQHandler(void)
// {
// 	if(EXTI_GetITStatus(EXTI_Line4) == SET)//接上文，只有多pin对应一通道的情况要判断
// 	{
// 		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 1)
// 		{
			
// 		}
// 		EXTI_ClearITPendingBit(EXTI_Line4);
// 	}
// }
