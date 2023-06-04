#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	/* Enable RCC APB1 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	/* Use internal clock */
	TIM_InternalClockConfig(TIM2);
	
	/* Timer init */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;//这里两个加一相乘后为72M/1K，共计72M/1K个数，即一次计时为10ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 -1;//而且这两个的取值都要在0~65535之间，但是这里没听懂
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//接上文，现在懂了，period是自动重装值，计数到达此值后重新计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);//prescaler是预分频值，晶振分频值后计数值加一，分频越多计数越慢
	
	/* Enable timer update IT */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//这一句是在初始化后清零标志位，从零开始计数
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	/* Set NVIC priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;//一点理解：好像初始化结构体都是这样的，先定义结构体
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//再初始化各种参数
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);//最后调用初始化函数
	
	/* Enable timer */
	TIM_Cmd(TIM2, ENABLE);
}

//void TIM2_IRQHandler(void)//这个中断函数在启动文件里找,而且中断函数可以放在使用它的地方
//{
//	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//	{
//		
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//	}
//}
