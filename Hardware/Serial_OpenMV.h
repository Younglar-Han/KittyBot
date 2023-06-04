#ifndef __SERIAL_OPENMV_H
#define __SERIAL_OPENMV_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>

extern uint8_t ToOpenMV_TxPacket[];
extern uint8_t FromOpenMV_RxPacket[]; // 如果改数据类型的话这里也需要改

void OpenMV_Init(void);
void ToOpenMV_SendByte(uint8_t Byte);
void ToOpenMV_SendArray(uint8_t *Array, uint16_t Length);
void ToOpenMV_SendString(char *String);
uint32_t OpenMV_Pow(uint32_t X, uint32_t Y);
void ToOpenMV_SendNumber(uint32_t Number, uint8_t Length);

void ToOpenMV_SendPacket(void);

uint8_t FromOpenMV_GetRxFlag(void);

#endif
