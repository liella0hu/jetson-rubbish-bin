#include "stm32f10x.h"                  // Device header
#include  "encoder.h"
#include "pwm.h"
#include "speed.h"

void TIM1_PWM_Init(u16 psc,u16 arr)		//TIM1_PWM_Init(72-1,100-1);    72MHz/72=1MHZ�ļ���Ƶ��  	pwmƵ�� 1MHZ/100 = 10Khz
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	//ʹ��GPIO����ʱ��ʹ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  
   
	
	//TIM_CH1��4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_11; 
	//�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 
	TIM_TimeBaseStructure.TIM_Period = arr; 
	//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	//TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 


	//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_Pulse = 0; 
	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	//ʹ������Ƚ�N״̬
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	//�� MOE=0 ���� TIM1 ����ȽϿ���״̬
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//�� MOE=0 ���� TIM1 ����Ƚ� N ����״̬
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	//�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	
	//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx	
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  
	
	
	//CH1Ԥװ��ʹ��	 
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  
	//CH4Ԥװ��ʹ��
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  
	//�������ָͨ���ϵĵ��¼�����ʱ�����޸ĵ�CCRx������Ƚϣ�����ֵ����Ӱ�ӼĴ���


	//ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ������޸ĵ�arr��ֵҪ���¼�����(���ڹ���)���ٽ���ֵ����Ĵ���
	TIM_ARRPreloadConfig(TIM1, ENABLE); 
	
	//MOE �����ʹ��	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	
	
	
	//ʹ��TIM1
	TIM_Cmd(TIM1, ENABLE);  
	
	
//	//ʱ�ӿ���
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//  
//  //��������
//  GPIO_InitTypeDef GPIO_InitStructure;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //�������
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;		
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);			
//  
//  //��ʱ��ģʽ����
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
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //�ߵ�ƽ��Ч
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = 0;     //CCR
//  TIM_OC1Init(TIM1, &TIM_OCInitStructure);    //TIM���ͨ����ʼ�� 
//  TIM_OC4Init(TIM1, &TIM_OCInitStructure);    //TIM���ͨ����ʼ��
//  
//  TIM_Cmd(TIM1, ENABLE);
}

void TIM2_PWM_Init(u16 arr,u16 psc)
{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //�������
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
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //�ߵ�ƽ��Ч
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);      //��ʼ������Ƚ�ͨ��
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
	TIM_SetCompare1(TIM2, Compare);//�ı�ARR��ֵ
}

void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(TIM2, Compare);//�ı�ARR��ֵ
}

void PWM_SetPrescaler(uint16_t Prescaler)      //д��PSC��ֵ
{
	TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);
}


