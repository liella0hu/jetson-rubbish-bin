#include "stm32f10x.h"                  // Device header
#include "pwm.h"
/*

一个端口接PWM，还有一个接高低电平，无论接哪个电平输出的PWM波形不变，若PWM输出30
IO接负：PWM的上半段输出电压，则给电机30的电压，正转
IO接正；PWM的上半段输出电压，则给电机70的电压，负转

*/

/*
左轮 B12+B13
PWM　A0
编码器		B6+B7

右轮 B14+B15
PWM　A1
编码器		A6+A7
*/

void Motor_Init()
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //APB2使能
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;          //推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;     //初始化PA2和PA3，用来输出高低电平               
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13); 
	GPIO_ResetBits(GPIOB,GPIO_Pin_14); 
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);   //拉低电平

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
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);   //拉低电平
}

