#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include <stdlib.h>
#include <math.h>
#include <motor.h>


//void Motor_Init(void)
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	PWM_Init();
//}

void Motor_SetSpeed(int Speed)
{
	if (Speed >= 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_10);
		GPIO_ResetBits(GPIOA, GPIO_Pin_11);
		//PWM_SetCompare3(Speed);
		TIM_SetCompare3(TIM2, Speed);				//���ö�ʱ��2��ͨ��վ�ıȽ�ֵ
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
		GPIO_SetBits(GPIOA, GPIO_Pin_11);
		TIM_SetCompare3(TIM2, Speed);				//���ö�ʱ��2��ͨ��վ�ıȽ�ֵ
	}
}
//void Motor_SetSpeed(int Speed)
//{
//	if (Speed >= 0)
//	{
//		GPIO_SetBits(GPIOA, GPIO_Pin_4);
//		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
//		//PWM_SetCompare3(Speed);
//		TIM_SetCompare3(TIM2, Speed);				//���ö�ʱ��2��ͨ��վ�ıȽ�ֵ
//	}
//	else
//	{
//		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
//		GPIO_SetBits(GPIOA, GPIO_Pin_5);
//		TIM_SetCompare3(TIM2, Speed);				//���ö�ʱ��2��ͨ��վ�ıȽ�ֵ
//	}
//}
int Basis_CCR=590;   //ʵ�ʲ��Եĵ��������������Сռ�ձ�

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;  //PA10��PA11Ϊ���Ʒ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	PWM_Init2();
}




//void Motor_Control(int TIM2_CCR)
//{
//	if(TIM2_CCR>0)
//	{
//		GPIO_SetBits(GPIOA, GPIO_Pin_10);      	 //��ת
//		GPIO_ResetBits(GPIOA, GPIO_Pin_11);	
//	}
//	else
//	{
//	  GPIO_ResetBits(GPIOA, GPIO_Pin_10);      //��ת
//		GPIO_SetBits(GPIOA, GPIO_Pin_11);
//	}
//	if(TIM2_CCR)
//	{
//		TIM2_CCR=abs(TIM2_CCR)+Basis_CCR; 			//��������Ļ���ռ�ձ�
//		if(TIM2_CCR>1000)    //�޷�
//			TIM2_CCR=999;
//		TIM_SetCompare3(TIM2, TIM2_CCR);				//���ö�ʱ��2��ͨ��վ�ıȽ�ֵ
//	
//	}
//	else
//	{
//		TIM_SetCompare3(TIM2, 0);
//		GPIO_SetBits(GPIOA, GPIO_Pin_10);//ɲ��
//		GPIO_SetBits(GPIOA, GPIO_Pin_11);
//	}

//}
