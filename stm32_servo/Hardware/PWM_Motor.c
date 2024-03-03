#include "stm32f10x.h"                  // Device header

void PWM_Motor_Init(void)
{
	//开启TIM2连接的总线APBB1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
 
	
	GPIO_InitTypeDef GPIOA_InitStructure;
	GPIOA_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIOA_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIOA_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIOA_InitStructure);
	
	//选择内部时钟
	TIM_InternalClockConfig(TIM2);
	
	//时基单元初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_TimeBaseStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period=100-1;   //ARR
	TIM_TimeBaseStruct.TIM_Prescaler=36-1;  //PSC
	TIM_TimeBaseStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStruct);
	
	TIM_OCInitTypeDef TIM_OCInitStructture;
	TIM_OCStructInit(&TIM_OCInitStructture);
	TIM_OCInitStructture.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructture.TIM_OCPolarity=TIM_OCPolarity_High ;
	TIM_OCInitStructture.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructture.TIM_Pulse=90;    //CCR
	TIM_OC3Init(TIM2,&TIM_OCInitStructture);
	TIM_Cmd(TIM2,ENABLE);
}
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2,Compare);
}

