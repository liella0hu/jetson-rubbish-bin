#include "sys.h"	
#include "delay.h"	
#include "InfrarePSwitch.h"

//初始化PF1口读取光电开关电平
void InfrarePSwitch_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10| GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
}
//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)	
//GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)	
//GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)
//GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)


int* InfrarePSwitch_Check(void)
{
	static int num[4];
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)	== 0)
	{
		num[0] = 0;
		
	}else
	{
		num[0] = 1;
	}
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)	== 0)
	{
		num[1] = 0;
		
	}else
	{
		num[1] = 1;
	}
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)	== 0)
	{
		num[2] = 0;
		
	}else
	{
		num[2] = 1;
	}
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)	== 0)
	{
		num[3] = 0;
		
	}else
	{
		num[3] = 1;
	}
	return num;
}

