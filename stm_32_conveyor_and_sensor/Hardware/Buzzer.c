#include "stm32f10x.h"                  // Device header

#define Buzzer_Pin GPIO_Pin_5
#define Buzzer_Pin_Port GPIOA
#define Buzzer_RCC RCC_APB2Periph_GPIOA

void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(Buzzer_RCC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = Buzzer_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Buzzer_Pin_Port, &GPIO_InitStructure);
	
	GPIO_ResetBits(Buzzer_Pin_Port, Buzzer_Pin);
}

void Buzzer_OFF(void)
{
	GPIO_ResetBits(Buzzer_Pin_Port, Buzzer_Pin);
}

void Buzzer_ON(void)
{
	GPIO_SetBits(Buzzer_Pin_Port, Buzzer_Pin);
}

void Buzzer_Turn(void)
{
	if (GPIO_ReadOutputDataBit(Buzzer_Pin_Port, Buzzer_Pin) == 0)
	{
		GPIO_SetBits(Buzzer_Pin_Port, Buzzer_Pin);
	}
	else
	{
		GPIO_ResetBits(Buzzer_Pin_Port, Buzzer_Pin);
	}
}
