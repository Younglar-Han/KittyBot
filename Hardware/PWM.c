#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	/* Enable RCC clock*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* GPIO init*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;//ARR //这里两个加一相乘后为72M，共计72M个数，即一次计时为一秒
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;//PSC //而且这两个的取值都要在0~65535之间，但是这里没听懂
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//接上文，现在懂了，period是自动重装值，计数到达此值后重新计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);//prescaler是预分频值，晶振分频值后计数值加一，分频越多计数越慢
	//这里PWM的频率是72M/72/100 = 10KHz（10KHz控制更准确且噪音小）
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//调用初始化函数给这个结构体的所有成员都赋初始值，防止出现错误
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;        //CCR
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)//设置TIM3_CH1 PA6 左PWM值（满值为100）
{
	TIM_SetCompare1(TIM3, Compare);
}

void PWM_SetCompare2(uint16_t Compare)//设置TIM3_CH2 PA7 右PWM值（满值为100）
{
	TIM_SetCompare2(TIM3, Compare);
}
