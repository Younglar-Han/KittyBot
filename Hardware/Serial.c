#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Motor.h"

uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[3];//除去包头包尾的数组长度但是因为有校验位所以比数据长度多1
uint8_t Serial_RxFlag;

extern float RemoteForwardSpeed;
extern float RemoteRotateRadSpeed;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//定义两个不同模式的引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);//USART初始化
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);//USART初始化
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	USART_GetFlagStatus(USART1, USART_FLAG_TXE);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) ==  RESET);//等待清零
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for(i = 0; i < Length; i++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for(i = 0; String[i] != '\0'; i++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while(Y--)
	{
		Result *= X;
	}
	return Result;
}
	

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i = 0; i < Length; i++)
	{
	Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format,arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket, 4);
	Serial_SendByte(0xFE);
	
}

void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		
		if (RxState == 0)
		{
			if (RxData == 0xA5)//包头
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket++;
			if (pRxPacket >= 3)//这里根据数组长度改变（除去包头包尾，因为有校验位所以比数据多1）
			{
				RxState = 2;
			}
		}else if (RxState == 2)
		{
			if (RxData == 0x5A)//包尾
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		}

		if(Serial_GetRxFlag() == 1)
		{
			//下面这一段转换的解释
			//由于蓝牙APP的原因，APP发送-1这里会接收到255，所以减去256刚好
			//APP发送的0~127是正常的，但发送的-128~-1会对应为128~255(APP好像是以补码形式发送的)
			//因此下面这一段实际上是取补码
			//1000 0010取补码1111 1110两者相加得1 0000 0000
			if(Serial_RxPacket[0] >= 0x80)//如果第一位是1则减去模长(256)(相当于取补码?)-(128-（符号位后的原数-128))
			{
				RemoteRotateRadSpeed = -(Serial_RxPacket[0] - 0x100);//因为遥控左右与角速度正好相反所以这里取负值
			}else//如果第一位是0就不做任何操作,见上文，指二进制码的第一位
			{
				RemoteRotateRadSpeed = -Serial_RxPacket[0];
			}
			if(Serial_RxPacket[1] >= 0x80)
			{
				RemoteForwardSpeed = (Serial_RxPacket[1] - 0x100);
			}else
			{
				RemoteForwardSpeed = Serial_RxPacket[1];
			}

			//这里速度设置为-100~100，映射为转向速度最大值为10rad/s，前进速度最大值为1.3m/s
			RemoteRotateRadSpeed = RemoteRotateRadSpeed/100.0f*10.0f;//转向速度最大值为10rad/s
			RemoteForwardSpeed = RemoteForwardSpeed/100.0f*1.3f;//前进速度最大值为1.3m/s

		}

//		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//		在前面GetITStatus的时候自动清除了标志位，这里不需要重复清除，否则可能会出现无法进入GetITStatus的情况
//		另外前面的GetITStatus写成GetFlagStatus了，但是之前居然一直能用
	}
}
