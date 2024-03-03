#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "Servo.h"
#include "Key.h"
#include "SimpleProtocol.h"
#include "usart.h"
#include "HC_SR04.h"
#include "timer.h"
#include "motor.h"

uint8_t KeyNum;
uint8_t RxData;
float Angle[8];
float Angle_level;
float Angle_vertical;
float offset_level;
float offset_vertical;
float Distance=0;
int8_t object_item;

struct UesrData{	
	float varf32s_1[6];
	float varf32s_2[6];
	float varf32s_3[6];
	int8_t vari8;
	
	//... //用户可加或者修改其他数据，总长度一般不超过125的话没事。
};
struct UesrData *recv_data_ptr1;
struct UesrData send_data1;

void homing()
{		//1水平，2垂直
		
		Servo_SetAngle1(90);
		
		Servo_SetAngle2(100);
}

int main(void)
{
	void Delay_ms(uint32_t xms);
	//OLED_Init();
	//PWM_Init();
	//Motor_Init();
	//Servo_Init();
	//Key_Init();
	uart1_init(115200);
	//HC_SR04_IO_Init();  //超声波模块GPIO初始化
	//Delay_ms(500);
	//TIM2_Init(7199,0);    //以10KHz计数,定时100us
	//Delay_ms(100);

	u16 len1=0, len_send=0;
	int i,flag=1;
	int t;
	int location_flag=1;
	OLED_ShowString(1, 1, "KeyNum:");
//	OLED_ShowString(2, 1, "Angle:");
//	OLED_ShowString(3, 1, "HEX:");
//	
	homing();
	while (1)
	{	
	
		len1=SimPro_UnPacking(SIMPRO1);
		
		//	发送	send_data1.vari8s[0]=66;
		Distance = (Get_SR04_Distance() * 331) * 1.0/1000;
		
		//send_data1.varf32s_1[0]=Distance;
		send_data1.varf32s_1[1]=66.6;
		len_send=SimPro_Packet(SIMPRO1,&send_data1,sizeof(send_data1));
		for(t=0;t<len_send;t++)
		{
			USART_SendData(USART1, SIMPRO1->send_buf[t]);//向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			Delay_ms(10);
		}
		
		
		if(len1>0)//成功解析出数据
		{			
				if(len1==sizeof(struct UesrData)){//检查收到的串长度是否符合长度，避免数组越界
						recv_data_ptr1 = (struct UesrData*)SIMPRO1->data_buf;//指向解析缓冲区即可
						object_item = recv_data_ptr1->vari8;
						}
				//发送数据
					
				 
		}
		
		if(object_item==2&&Distance<=100){Servo_SetAngle1(5);Delay_ms(200);Servo_SetAngle2(70);Delay_ms(200);homing();}//brick
		if(object_item==4&&Distance<=100){Servo_SetAngle1(50);Delay_ms(200);Servo_SetAngle2(70);Delay_ms(200);homing();}//carrot
		if(object_item==6&&Distance<=100){Servo_SetAngle1(95);Delay_ms(200);Servo_SetAngle2(70);Delay_ms(200);homing();}//medicine
		if(object_item==7&&Distance<=100){Servo_SetAngle1(140);Delay_ms(200);Servo_SetAngle2(70);Delay_ms(200);homing();}//porcelain
		if(object_item==8&&Distance<=100){Servo_SetAngle1(186);Delay_ms(200);Servo_SetAngle2(70);Delay_ms(200);homing();}//potato
		if(object_item==9&&Distance<=100){Servo_SetAngle1(230);Delay_ms(200);Servo_SetAngle2(70);Delay_ms(200);homing();}//stone
		
		

			
//PWM_SetCompare3(50);
//		Servo_SetAngle1(80);
//		Delay_ms(500);
//		Servo_SetAngle1(90);
//		Delay_ms(500);
		

		
		

	}
}


