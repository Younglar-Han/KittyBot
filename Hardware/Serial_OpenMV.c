#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "LED.h"

uint8_t ToOpenMV_TxPacket[4];
uint8_t FromOpenMV_RxPacket[6];//除去包头包尾的数组长度但是因为有校验位所以比数据长度多1
uint8_t FromOpenMV_RxFlag;

void OpenMV_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//定义两个不同模式的引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStructure);//USART初始化
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART3, ENABLE);//USART初始化
	LED_Init();
}

void ToOpenMV_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);
	USART_GetFlagStatus(USART3, USART_FLAG_TXE);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) ==  RESET);//等待清零
}

void ToOpenMV_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for(i = 0; i < Length; i++)
	{
		ToOpenMV_SendByte(Array[i]);
	}
}

void ToOpenMV_SendString(char *String)
{
	uint8_t i;
	for(i = 0; String[i] != '\0'; i++)
	{
		ToOpenMV_SendByte(String[i]);
	}
}

uint32_t OpenMV_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while(Y--)
	{
		Result *= X;
	}
	return Result;
}
	

void ToOpenMV_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i = 0; i < Length; i++)
	{
	ToOpenMV_SendByte(Number / OpenMV_Pow(10, Length - i - 1) % 10 + '0');
	}
}

uint8_t FromOpenMV_GetRxFlag(void)
{
	if (FromOpenMV_RxFlag == 1)
	{
		FromOpenMV_RxFlag = 0;
		return 1;
	}
	return 0;
}

void ToOpenMV_SendPacket(void)
{
	ToOpenMV_SendByte(0xFF);
	ToOpenMV_SendArray(ToOpenMV_TxPacket, 4);
	ToOpenMV_SendByte(0xFE);
	
}

void USART3_IRQHandler(void)
{
	static uint8_t FromOpenMV_RxState = 0;
	static uint8_t FromOpenMV_pRxPacket = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		uint8_t FromOpenMV_RxData = USART_ReceiveData(USART3);
		
		if (FromOpenMV_RxState == 0)
		{
			if (FromOpenMV_RxData == 0xA5)//包头
			{
				FromOpenMV_RxState = 1;
				FromOpenMV_pRxPacket = 0;
			}
		}else if (FromOpenMV_RxState == 1)
		{
			FromOpenMV_RxPacket[FromOpenMV_pRxPacket] = FromOpenMV_RxData;
			FromOpenMV_pRxPacket++;
			if (FromOpenMV_pRxPacket >= 6)//这里根据数组长度改变（除去包头包尾，因为有校验位所以比数据多1）
			{
				FromOpenMV_RxState = 2;
			}
		}else if (FromOpenMV_RxState == 2)
		{
			if (FromOpenMV_RxData == 0x5A)//包尾
			{
				FromOpenMV_RxState = 0;
				FromOpenMV_RxFlag = 1;
			}
		}
		if(FromOpenMV_RxPacket[5] == 1)
		{
			RedLED_Turn();
		}
		
		
//		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//		在前面GetITStatus的时候自动清除了标志位，这里不需要重复清除，否则可能会出现无法进入GetITStatus的情况
//		另外前面的GetITStatus写成GetFlagStatus了，但是之前居然一直能用
	}
}
