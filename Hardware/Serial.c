#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Motor.h"

uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[3];//��ȥ��ͷ��β�����鳤�ȵ�����Ϊ��У��λ���Ա����ݳ��ȶ�1
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
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//����������ͬģʽ������
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
	USART_Init(USART1, &USART_InitStructure);//USART��ʼ��
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);//USART��ʼ��
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	USART_GetFlagStatus(USART1, USART_FLAG_TXE);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) ==  RESET);//�ȴ�����
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
			if (RxData == 0xA5)//��ͷ
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket++;
			if (pRxPacket >= 3)//����������鳤�ȸı䣨��ȥ��ͷ��β����Ϊ��У��λ���Ա����ݶ�1��
			{
				RxState = 2;
			}
		}else if (RxState == 2)
		{
			if (RxData == 0x5A)//��β
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		}

		if(Serial_GetRxFlag() == 1)
		{
			//������һ��ת���Ľ���
			//��������APP��ԭ��APP����-1�������յ�255�����Լ�ȥ256�պ�
			//APP���͵�0~127�������ģ������͵�-128~-1���ӦΪ128~255(APP�������Բ�����ʽ���͵�)
			//���������һ��ʵ������ȡ����
			//1000 0010ȡ����1111 1110������ӵ�1 0000 0000
			if(Serial_RxPacket[0] >= 0x80)//�����һλ��1���ȥģ��(256)(�൱��ȡ����?)-(128-������λ���ԭ��-128))
			{
				RemoteRotateRadSpeed = -(Serial_RxPacket[0] - 0x100);//��Ϊң����������ٶ������෴��������ȡ��ֵ
			}else//�����һλ��0�Ͳ����κβ���,�����ģ�ָ��������ĵ�һλ
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

			//�����ٶ�����Ϊ-100~100��ӳ��Ϊת���ٶ����ֵΪ10rad/s��ǰ���ٶ����ֵΪ1.3m/s
			RemoteRotateRadSpeed = RemoteRotateRadSpeed/100.0f*10.0f;//ת���ٶ����ֵΪ10rad/s
			RemoteForwardSpeed = RemoteForwardSpeed/100.0f*1.3f;//ǰ���ٶ����ֵΪ1.3m/s

		}

//		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//		��ǰ��GetITStatus��ʱ���Զ�����˱�־λ�����ﲻ��Ҫ�ظ������������ܻ�����޷�����GetITStatus�����
//		����ǰ���GetITStatusд��GetFlagStatus�ˣ�����֮ǰ��Ȼһֱ����
	}
}
