#include "stm32f10x.h"                  // Device header
#include "motor.h"
#include "Delay.h"
#include "PID.H"

//extern int sensor[3];//����������main���������
//extern int16_t speed1,speed2,tar1,tar2;


void huidu_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��GPIOA��ʱ��
	
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//ʹ�ܸ��ù��ܵ�ʱ��
	
	  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//IO�ܽ�ģʽ����Ϊ�������룬�������ܻ�ȡ�������������������ź�(�ߵ͵�ƽ)
	  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
		
	  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	  GPIO_Init(GPIOA,&GPIO_InitStructure);//PA4,PA5,PA6,PA7�ܽŵĳ�ʼ��
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	  
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    //APB2ʹ��
	
	
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;       
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;                   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//�ر�STM32f103c8t6��JTAGD���ܣ�ʹPB3��PB4������ͨ��IO�ܽ�//���뿪�����ù��ܵ�ʱ�Ӳ��ܹرոù���
}


//int16_t read_sensor(int16_t hui,int16_t num)
//{
//		
//		sensor[0]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3);//����Ĵ�����
//		sensor[1]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
//		sensor[2]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);//���ҵĴ�����	
//	
//		if(sensor[0] == 0 && sensor[1] == 1 && sensor[2] == 0)//ʶ���м�
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
