#include "stm32f10x.h"                  // Device header
#include  "encoder.h"
#include "pwm.h"
#include "speed.h"

void TIM1_PWM_Init(u16 psc,u16 arr)		//TIM1_PWM_Init(72-1,100-1);    72MHz/72=1MHZ的计数频率  	pwm频率 1MHZ/100 = 10Khz
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	//使能GPIO外设时钟使能
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
   
	
	//TIM_CH1、4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_11; 
	//复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 
	TIM_TimeBaseStructure.TIM_Period = arr; 
	//设置用来作为TIMx时钟频率除数的预分频值  不分频
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	//TIM向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 


	//选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	//设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_Pulse = 0; 
	//比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	//使能输出比较N状态
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	//当 MOE=0 重置 TIM1 输出比较空闲状态
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//当 MOE=0 重置 TIM1 输出比较 N 空闲状态
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	//输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	
	//根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	//根据TIM_OCInitStruct中指定的参数初始化外设TIMx	
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  
	
	
	//CH1预装载使能	 
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  
	//CH4预装载使能
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  
	//上面个是指通道上的当事件更新时，将修改的CCRx（输出比较）的数值放入影子寄存器


	//使能TIMx在ARR上的预装载寄存器，修改的arr的值要等事件更新(周期过后)后再将数值放入寄存器
	TIM_ARRPreloadConfig(TIM1, ENABLE); 
	
	//MOE 主输出使能	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	
	
	
	//使能TIM1
	TIM_Cmd(TIM1, ENABLE);  
	
	
//	//时钟开启
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//  
//  //引脚配置
//  GPIO_InitTypeDef GPIO_InitStructure;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //推挽输出
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;		
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);			
//  
//  //定时器模式配置
//  TIM_InternalClockConfig(TIM1);
//  
//  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//  TIM_TimeBaseInitStructure.TIM_Period = arr;     //ARR 
//  TIM_TimeBaseInitStructure.TIM_Prescaler = psc;   //PSC 
//  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
//  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
//  
//  TIM_OCInitTypeDef TIM_OCInitStructure;
//  TIM_OCStructInit(&TIM_OCInitStructure);         
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //高电平有效
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = 0;     //CCR
//  TIM_OC1Init(TIM1, &TIM_OCInitStructure);    //TIM输出通道初始化 
//  TIM_OC4Init(TIM1, &TIM_OCInitStructure);    //TIM输出通道初始化
//  
//  TIM_Cmd(TIM1, ENABLE);
}

void TIM2_PWM_Init(u16 arr,u16 psc)
{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);         //
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //高电平有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);      //初始化书橱比较通道
    TIM_OC2Init(TIM2, &TIM_OCInitStructure); 
    
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM2, Compare);//改变ARR的值
}

void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(TIM2, Compare);//改变ARR的值
}

void PWM_SetPrescaler(uint16_t Prescaler)      //写入PSC的值
{
	TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);
}


