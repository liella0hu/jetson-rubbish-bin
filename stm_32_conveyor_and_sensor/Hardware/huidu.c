#include "stm32f10x.h"                  // Device header
#include "motor.h"
#include "Delay.h"
#include "PID.H"

//extern int sensor[3];//引用主函数main定义的数组
//extern int16_t speed1,speed2,tar1,tar2;


void huidu_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA的时钟
	
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能复用功能的时钟
	
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//IO管脚模式配置为浮空输入，这样就能获取传感器传回来的数字信号(高低电平)
	  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
		
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	  GPIO_Init(GPIOA,&GPIO_InitStructure);//PA4,PA5,PA6,PA7管脚的初始化
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	  
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //APB2使能
	
	
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;       
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;                   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭STM32f103c8t6的JTAGD功能，使PB3和PB4用作普通的IO管脚//必须开启复用功能的时钟才能关闭该功能
}


//int16_t read_sensor(int16_t hui,int16_t num)
//{
//		
//		sensor[0]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3);//最左的传感器
//		sensor[1]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
//		sensor[2]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);//最右的传感器	
//	
//		if(sensor[0] == 0 && sensor[1] == 1 && sensor[2] == 0)//识别到中间
//		{
//			hui=1;
//			
//		}

//		if(sensor[0] == 0 && sensor[1] == 1 && sensor[2] == 1)//
//		{
//			hui=2;
//			
//		}

//		if(sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 0)
//		{
//			hui=3;
//			
//		}
//		
//		if(sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 1)
//		{
//			hui=4;
//			
//		}

//		if(sensor[0] == 1 && sensor[1] == 0 && sensor[2] == 0)
//		{
//			hui=5;
//			
//		}		
//		if(sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0)
//		{
//			hui=6;
//			
//		}
//		
//		if(sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1)
//		{
//			num++;
//			hui=1;
//		}
//		
//		return hui;
//}
