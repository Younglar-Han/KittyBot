#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include "PWM.h"
#include "Encoder.h"
#include "PID.h"

#define Pi 3.14159265358979
const float d = 0.13;//小车两轮间距为0.13m

uint8_t SPD_MODE = 0;
uint8_t POS_MODE = 1;

/* 位置PID的参数，输出为电压百分比(没写串级) */
const float Position_Kp = 1000.0f;
const float Position_Ki = 0.0f;
const float Position_Kd = 0.0f;
const float Position_PID_Parameters [3] = {Position_Kp, Position_Ki, Position_Kd};
float Position_VolPercent_Max_out = 70.0f;//位置控制PID电压百分比最大输出
float Position_VolPercent_Max_iout = 30.0f;

/* 速度PID的参数，输出为电压百分比 */
const float Speed_Kp = 200.0f;
const float Speed_Ki = 0.0f;
const float Speed_Kd = 10.0f;
const float Speed_PID_Parameters [3] = {Speed_Kp, Speed_Ki, Speed_Kd};
float Speed_VolPercent_Max_out = 100.0f;//速度控制PID电压百分比最大输出
float Speed_VolPercent_Max_iout = 30.0f;


float LastPosition1;
float Position1Ref;
float Position1Set;
float LastPosition2;
float Position2Ref;
float Position2Set;

float Speed1Ref; //left speed feedback
float Speed2Ref; //right speed feedback
float Speed1Set;
float Speed2Set;

float LinearXRef;
float AngularZRef;

float Motor1_VolPercentSet;
float Motor2_VolPercentSet;

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	/* Disable JLink, enable SW */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//由于A15 B3 B4默认主功能为 JTDI JTDO NJTRST,在当作一般IO口之前需要禁用JLink(重映射)
	
	PWM_Init();
}

void Motor1_SetVoltagePercent(float Percent)
{
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);   
	/* Disable JLink, enable SW */
	/* 这三个引脚的配置需要放在所有IO口的配置之后，这里保险起见再次失能JLink */
	if (Percent >= 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_15);
		GPIO_ResetBits(GPIOB, GPIO_Pin_3);
		PWM_SetCompare1(Percent);
	}else{
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);
		GPIO_SetBits(GPIOB, GPIO_Pin_3);
		PWM_SetCompare1(-Percent);
	}
}

void Motor2_SetVoltagePercent(float Percent)
{
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    
	/* Disable JLink, enable SW */
	/* 这三个引脚的配置需要放在所有IO口的配置之后，这里保险起见再次失能JLink */
	if (Percent >= 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_4);
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		PWM_SetCompare2(Percent);
	}else{
		GPIO_ResetBits(GPIOB, GPIO_Pin_4);
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
		PWM_SetCompare2(-Percent);
	}
}

/* 驱动电机1前进对应距离 */
void Motor1_RunPosition(float RunPosition)
{
	Position1Set = Position1Ref + RunPosition;
	// Position1Set += RunPosition;
}
/* 驱动电机2前进对应距离 */
void Motor2_RunPosition(float RunPosition)
{
	Position2Set = Position2Ref + RunPosition;
	// Position2Set += RunPosition;
	//这里好像5.2上午有一次逻辑问题，但是忘了怎么回事了
}

void Motor1_SpeedSet(float Speed)
{
	Speed1Set = Speed;
}

void Motor2_SpeedSet(float Speed)
{
	Speed2Set = Speed;
}

float Speed1_Get(void)
{
	return Speed1Ref;
}
float Speed2_Get(void)
{
	return Speed2Ref;
}

/* 驱动小车前进对应距离 */
void ForwardDistance(float ForwardDistance)
{
	Motor1_RunPosition(ForwardDistance);
	Motor2_RunPosition(ForwardDistance);
	POS_MODE = 1;
	SPD_MODE = 0;
}

/* 驱动小车旋转对应弧度 */
void YawRotateRad(float RotateRad)
{
	Motor1_RunPosition(-RotateRad*d/2);
	Motor2_RunPosition(RotateRad*d/2);
	POS_MODE = 1;
	SPD_MODE = 0;
}

/* 驱动小车旋转对应角度 */
void YawRotateDeg(float RotateDeg)
{
	float RotateRad = RotateDeg/180*Pi;
	Motor1_RunPosition(-RotateRad*d/2);
	Motor2_RunPosition(RotateRad*d/2);
	POS_MODE = 1;
	SPD_MODE = 0;
}

/* 驱动小车以对应速度边前进边转向 单位m/s, rad/s */
void CarSpeedSet(float ForwardSpeed, float RotateRadSpeed)
{
	Motor1_SpeedSet(ForwardSpeed-RotateRadSpeed*d/2);
	Motor2_SpeedSet(ForwardSpeed+RotateRadSpeed*d/2);
	POS_MODE = 0;
	SPD_MODE = 1;
}

void Motor_Update(void)
{
	/* PID init*/
	pid_type_def Motor_PositionPID;//Position PID
	PID_init(&Motor_PositionPID, PID_POSITION, Position_PID_Parameters, Position_VolPercent_Max_out, Position_VolPercent_Max_iout);
	pid_type_def Motor_SpeedPID;//Speed PID
	PID_init(&Motor_SpeedPID, PID_POSITION, Speed_PID_Parameters, Speed_VolPercent_Max_out, Speed_VolPercent_Max_iout);
	
	/* PositionRef update */
	LastPosition1 = Position1Ref;//Be careful not to use PositionGet elsewhere, otherwise the speed calculation will be wrong
	LastPosition2 = Position2Ref;//Ensure that the PositionRef is updated once at 10ms
	Position1Ref = Position1_Get();
	Position2Ref = Position2_Get();

	/* SpeedRef update */
	Speed1Ref = (Position1Ref-LastPosition1)*100;//(m/s)
	Speed2Ref = (Position2Ref-LastPosition2)*100;//(m/s)
	LinearXRef = (Speed1Ref + Speed2Ref) / 2.0f;
	AngularZRef = (Speed2Ref - Speed1Ref) / d;
	
	if(POS_MODE)
	{
		Motor1_VolPercentSet = PID_calc(&Motor_PositionPID, Position1Ref, Position1Set);
		Motor1_SetVoltagePercent(Motor1_VolPercentSet);

		Motor2_VolPercentSet = PID_calc(&Motor_PositionPID, Position2Ref, Position2Set);
		Motor2_SetVoltagePercent(Motor2_VolPercentSet);
	}else if (SPD_MODE)
	{
		Motor1_VolPercentSet = PID_calc(&Motor_SpeedPID, Speed1Ref, Speed1Set);
		Motor1_SetVoltagePercent(Motor1_VolPercentSet);

		Motor2_VolPercentSet = PID_calc(&Motor_SpeedPID, Speed2Ref, Speed2Set);
		Motor2_SetVoltagePercent(Motor2_VolPercentSet);
	}
}
