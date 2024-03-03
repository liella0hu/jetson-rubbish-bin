#include "stm32f10x.h"                  // Device header
#include "pwm.h"
/*

һ���˿ڽ�PWM������һ���Ӹߵ͵�ƽ�����۽��ĸ���ƽ�����PWM���β��䣬��PWM���30
IO�Ӹ���PWM���ϰ�������ѹ��������30�ĵ�ѹ����ת
IO������PWM���ϰ�������ѹ��������70�ĵ�ѹ����ת

*/

/*
���� B12+B13
PWM��A0
������		B6+B7

���� B14+B15
PWM��A1
������		A6+A7
*/

void Motor_Init()
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //APB2ʹ��
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;          //�������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;     //��ʼ��PA2��PA3����������ߵ͵�ƽ               
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13); 
	GPIO_ResetBits(GPIOB,GPIO_Pin_14); 
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);   //���͵�ƽ

}

void left_f(uint16_t speed)       //A0+B12+B13     A8+B12+B13
{
    PWM_SetCompare1(speed);       
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);  
}
void left_b(uint16_t speed)       //A0+B12+B13
{
    PWM_SetCompare1(speed);       
    GPIO_SetBits(GPIOB, GPIO_Pin_13);
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	
}
void right_f(uint16_t speed)       //A1+B14+B15   A11+B14+B15
{
    PWM_SetCompare2(speed);       
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}
void right_b(uint16_t speed)       //A1+B14+B15
{
    PWM_SetCompare2(speed);       
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void Pull(void)
{
	//PWM_SetCompare2(100); 
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void Push(void)
{
	//PWM_SetCompare2(100); 
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void Stop(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_14); 
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);   //���͵�ƽ
}

