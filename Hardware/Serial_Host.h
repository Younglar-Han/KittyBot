#ifndef __SERIAL_HOST_H
#define __SERIAL_HOST_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>

extern uint8_t ToHost_TxPacket[];
extern uint8_t FromHost_RxPacket[]; //如果改数据类型的话这里也需要改

void Host_Init(void);
void ToHost_SendByte(uint8_t Byte);
void ToHost_SendArray(uint8_t *Array, uint16_t Length);
void ToHost_SendString(char *String);
uint32_t Host_Pow(uint32_t X, uint32_t Y);
void ToHost_SendNumber(uint32_t Number, uint8_t Length);

void ToHost_SendPacket(void);

uint8_t FromHost_GetRxFlag(void);

#endif
