#include "stm32f10x.h"                  // Device header

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void GreenLED_ON(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);//低电平点亮
}

void GreenLED_OFF(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void GreenLED_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == 0)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
	}else{
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}
}

