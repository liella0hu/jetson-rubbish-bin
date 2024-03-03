#include "stm32f10x.h"                  // Device header
#include "motor.h"
#include "pid.h"

 
void My_USART1_Init(void)  
{  
    	  
		GPIO_InitTypeDef gpioinstructure;//GPIO结构体初始化函数
	  USART_InitTypeDef usartinstructure;//USART结构体初始化函数
	  NVIC_InitTypeDef  nvicinstructure;//中断控制器结构体初始化函数
	  
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置中断控制器优先抢占级组
		
	//1.配置GPIO、USART、引脚复用时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//配置GPIOA时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//配置引脚复用时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//配置USART时钟
		
	//2.配置GPIO结构体
	   
    //配置PA9 TX 输出引脚
		gpioinstructure.GPIO_Mode  =  GPIO_Mode_AF_PP;//复用推挽输出
	  gpioinstructure.GPIO_Pin   =  GPIO_Pin_9 ;//引脚9
	  gpioinstructure.GPIO_Speed =  GPIO_Speed_50MHz;//速度为50Mhz
	
	  GPIO_Init(GPIOA,&gpioinstructure);//GPIO初始化
	
	  //配置PA10 RX 接收引脚
	  gpioinstructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;//浮空输出
	  gpioinstructure.GPIO_Pin   = GPIO_Pin_10;//引脚10
		
		GPIO_Init(GPIOA,&gpioinstructure);//GPIO初始化
		
	//3.配置串口的结构体
	  usartinstructure.USART_BaudRate = 9600;//波特率为9600
		usartinstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流配置
		usartinstructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx ;//接收模式
		usartinstructure.USART_Parity = USART_Parity_No;//无校验位
		usartinstructure.USART_StopBits = USART_StopBits_1;//一个停止位
		usartinstructure.USART_WordLength = USART_WordLength_8b;//有效数据位为8位
    
    USART_Init(USART1,&usartinstructure);//初始化串口1
    
    USART_Cmd(USART1,ENABLE);	//使能串口1
		
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//串口中断配置
		
	//4.配置中断控制器的结构
	  nvicinstructure.NVIC_IRQChannel  =  USART1_IRQn;//中断通道
		nvicinstructure.NVIC_IRQChannelCmd = ENABLE; //通道使能
		nvicinstructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级配置为1
		nvicinstructure.NVIC_IRQChannelSubPriority = 1;//子优先级配置为1
		
	  NVIC_Init(&nvicinstructure);//中断控制器初始化
			  
    
}  
  
//void control(u8 res,int16_t speed1,int16_t speed2,int16_t tar1,int16_t tar2)
//{
//        if(res==49)              //前进
//        {
//            left_f(pid1(speed1,tar1));
//            right_f(pid2(speed2,tar2));
//            
//        }
//        else if(res==48)            //停止
//        {
//            left_f(pid1(speed1,0));
//            right_f(pid2(speed2,0));
//            left_f(0);
//            right_f(0);
//        }
//        else if(res==50)                //后退
//        {
//            left_b(pid1(speed1,tar1));
//            right_b(pid2(speed2,tar2));
//            
//        }
//        else if(res==52)                //向左
//        {
//            left_f(pid1(speed1,0));
//            right_f(pid2(speed2,10));
//            
//        }
//         else if(res==51)                //向右
//        {
//            left_f(pid1(speed1,10));
//            right_f(pid2(speed2,0));
//          
//        }
//}

   
