#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>

extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[]; // 如果改数据类型的话这里也需要改

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
uint32_t Serial_Pow(uint32_t X, uint32_t Y);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

void Serial_SendPacket(void);

uint8_t Serial_GetRxFlag(void);

#endif
