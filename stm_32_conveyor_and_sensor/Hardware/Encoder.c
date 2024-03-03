#include "stm32f10x.h"                  // Device header
//���������������Լλ263


void Encoder1_Init_TIM3()
{
    //����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);      
    //����Ƚ��õ�TIM2���������벶����TIM3��������һ����TIM3Ҳ��APB1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    //����GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;   //TIM3��ͨ��һ��������PA6��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    //����ʱ����Ԫ
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;		//ARR����󣬷�ֹ���
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	TIM_ICInitTypeDef TIM_ICInitStructure;
  TIM_ICStructInit(&TIM_ICInitStructure);    //��Ĭ�ϵĳ�ʼֵ
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;         //ͨ��һ
	TIM_ICInitStructure.TIM_ICFilter = 0xF;                //�˲���С 
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;         //ͨ��һ
	TIM_ICInitStructure.TIM_ICFilter = 0xF;                //�˲���С  
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
    //���ñ������ӿ�
    TIM_Cmd(TIM3,ENABLE);
}



int16_t Encoder1_get(void)
{
    int16_t Temp;
    Temp=TIM_GetCounter(TIM3);
    TIM_SetCounter(TIM3,0);
    return Temp;
}



void Encoder2_Init_TIM4()
{
    //����ʱ��
	        
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
    //����GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;   //TIM3��ͨ��һ��������PA6��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    //����ʱ����Ԫ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;		//ARR����󣬷�ֹ���
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
	
	TIM_ICInitTypeDef TIM_ICInitStructure;
  TIM_ICStructInit(&TIM_ICInitStructure);    //��Ĭ�ϵĳ�ʼֵ
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;         //ͨ��һ
	TIM_ICInitStructure.TIM_ICFilter = 0xF;                //�˲���С 
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;         //ͨ��һ
	TIM_ICInitStructure.TIM_ICFilter = 0xF;                //�˲���С  
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
  TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
    //���ñ������ӿ�
    
  TIM_Cmd(TIM4,ENABLE);
}

int16_t Encoder2_get(void)
{
    int16_t Temp;
    Temp=TIM_GetCounter(TIM4);
    TIM_SetCounter(TIM4,0);
    return Temp;
}
