#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdio.h>

void Motor_Init(void);
void Motor1_SetVoltagePercent(float Speed);
void Motor2_SetVoltagePercent(float Speed);

void Motor1_RunPosition(float RunPosition);
void Motor2_RunPosition(float RunPosition);
void Motor1_SpeedSet(float Speed);
void Motor2_SpeedSet(float Speed);
float Speed1_Get(void);
float Speed2_Get(void);

void ForwardDistance(float ForwardDistance);
void YawRotateRad(float RotateRad);
void YawRotateDeg(float RotateDeg);

void CarSpeedSet(float Speed, float RotateDegSpeed);

void Motor_Update(void);
#endif
