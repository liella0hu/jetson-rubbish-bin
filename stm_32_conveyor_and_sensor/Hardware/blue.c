#include "stm32f10x.h"                  // Device header
#include "motor.h"
#include "pid.h"

 
void My_USART1_Init(void)  
{  
    	  
		GPIO_InitTypeDef gpioinstructure;//GPIO�ṹ���ʼ������
	  USART_InitTypeDef usartinstructure;//USART�ṹ���ʼ������
	  NVIC_InitTypeDef  nvicinstructure;//�жϿ������ṹ���ʼ������
	  
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����жϿ�����������ռ����
		
	//1.����GPIO��USART�����Ÿ���ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//����GPIOAʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�������Ÿ���ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//����USARTʱ��
		
	//2.����GPIO�ṹ��
	   
    //����PA9 TX �������
		gpioinstructure.GPIO_Mode  =  GPIO_Mode_AF_PP;//�����������
	  gpioinstructure.GPIO_Pin   =  GPIO_Pin_9 ;//����9
	  gpioinstructure.GPIO_Speed =  GPIO_Speed_50MHz;//�ٶ�Ϊ50Mhz
	
	  GPIO_Init(GPIOA,&gpioinstructure);//GPIO��ʼ��
	
	  //����PA10 RX ��������
	  gpioinstructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//�������
	  gpioinstructure.GPIO_Pin   = GPIO_Pin_10;//����10
		
		GPIO_Init(GPIOA,&gpioinstructure);//GPIO��ʼ��
		
	//3.���ô��ڵĽṹ��
	  usartinstructure.USART_BaudRate = 9600;//������Ϊ9600
		usartinstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
		usartinstructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx ;//����ģʽ
		usartinstructure.USART_Parity = USART_Parity_No;//��У��λ
		usartinstructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		usartinstructure.USART_WordLength = USART_WordLength_8b;//��Ч����λΪ8λ
    
    USART_Init(USART1,&usartinstructure);//��ʼ������1
    
    USART_Cmd(USART1,ENABLE);	//ʹ�ܴ���1
		
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//�����ж�����
		
	//4.�����жϿ������Ľṹ
	  nvicinstructure.NVIC_IRQChannel  =  USART1_IRQn;//�ж�ͨ��
		nvicinstructure.NVIC_IRQChannelCmd = ENABLE; //ͨ��ʹ��
		nvicinstructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�����Ϊ1
		nvicinstructure.NVIC_IRQChannelSubPriority = 1;//�����ȼ�����Ϊ1
		
	  NVIC_Init(&nvicinstructure);//�жϿ�������ʼ��
			  
    
}  
  
//void control(u8 res,int16_t speed1,int16_t speed2,int16_t tar1,int16_t tar2)
//{
//        if(res==49)              //ǰ��
//        {
//            left_f(pid1(speed1,tar1));
//            right_f(pid2(speed2,tar2));
//            
//        }
//        else if(res==48)            //ֹͣ
//        {
//            left_f(pid1(speed1,0));
//            right_f(pid2(speed2,0));
//            left_f(0);
//            right_f(0);
//        }
//        else if(res==50)                //����
//        {
//            left_b(pid1(speed1,tar1));
//            right_b(pid2(speed2,tar2));
//            
//        }
//        else if(res==52)                //����
//        {
//            left_f(pid1(speed1,0));
//            right_f(pid2(speed2,10));
//            
//        }
//         else if(res==51)                //����
//        {
//            left_f(pid1(speed1,10));
//            right_f(pid2(speed2,0));
//          
//        }
//}

   
