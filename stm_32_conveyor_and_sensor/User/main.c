#include "stm32f10x.h"  
#include "sys.h"               // Device header
#include "Delay.h"
#include "OLED.h"
#include "huidu.h"
//#include "serial.h"
#include "pwm.h"
#include "Timer.h"
#include "HCSR04.h"
#include "motor.h"
#include "stdio.h"
#include  "ENCODER.H"
#include "PID.H"
#include "speed.h"
#include "Buzzer.h"
#include <stdio.h>
#include <stdlib.h>
#include "uart_for_encode.h"
#include "usart.h"
#include "SimpleProtocol.h"
#include "LED.h"
#include "InfrarePSwitch.h"


#if 1
float tar=0.2;
float tar1=0.4;
float tar2 = 0.3;

int16_t pid_flag = 0;
int16_t left_encode_num = 0;
int16_t right_encode_num = 0;
float Lspeed = 0.0f;
float Rspeed = 0.0f;
float L_pwm = 0.0f;
float R_pwm = 0.0f;
float set_left_tar = 0.0f;
float set_right_tar = 0.0f;

float Kp_L = 60, Ki_L = 0.9;
float Kp_R = 60, Ki_R = 0.9;
float error_L = 0, out_L = 0, err_last_L = 0;
float error_R = 0, out_R = 0, err_last_R = 0;

float Position_KP_L = 5000, Position_KI_L = 0.2, Position_KD_L = 0.1; 
float Integral_error_L=0,error_last_L=0; 
float Position_KP_R = 15, Position_KI_R = 0.1, Position_KD_R =0 ;
float Integral_error_R=0,error_last_R=0; 


float Amend_tar=0,R,Angle=0,Arc_length=0;
float sensor_wid = 15,car_leng = 105,car_wid = 92.5;
float turn_angle=0,turn_line=0,R2=0,angle_speed_temp=0;

float angle_speed=0,distance= 0;

float sensor_cent_size = 0;
int coin;
int sensor[4]={0,0,0,0};   //定义一个数组，用于储存灰度传感器返回的值
//char Blueshow[2];

int turn=0;

int8_t Left_OR_Right = 0;

int8_t remember_number = 0,only_once = 1,remember_location;
int8_t confirm_number = 0;
int8_t flag1 = 1,flag2 = 1,flag3 =1;
int8_t count =0;

#endif






int Distance_mm[2]={0};			//获取距离测量结果，单位毫米（mm）		
int Distance_m[2]={0};	//转换为米（m）为单位，将整数部分放入Distance_m
int Distance_m_p[2]={0};	//转换为米（m）为单位，将小数部分放入Distance_m_puint64_t numlen(uint64_t num)//计算数字的长度
int delay_flag[8] = {0};
uint64_t numlen(uint64_t num);//计算数字的长度
void Show_distance(void);
void Full_Load_Warning(void);
int* signal;
int16_t oled_clear_flag = 0;

struct UesrData{	
//	float varf32s_1[6];
//	float varf32s_2[6];
//	float varf32s_3[6];
	int8_t vari82;
	int8_t vari8;
	
	//... //用户可加或者修改其他数据，总长度一般不超过125的话没事。
};
struct UesrData *recv_data_ptr1;
struct UesrData send_data1;

u16 len1=0, len_send=0;
int t;
int communicate=1;
int8_t object_item=10;
int8_t item_num = 4;
int8_t anti_trigger = 1;

int main(void)
{
		uart_init(115200);	 //串口初始化为115200
#if 1	
    Buzzer_Init();
    TIM2_PWM_Init(100-1,720-1);  // 定时周期  10ms  72Mhz/10 = 7200Khz  频率 7200Khz/7200 = 1Khz  周期1ms 		0.001s 
		LED_Init();
    Motor_Init();
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
		OLED_Init();		//初始化OLED屏
	  Timer_Init();		//初始化定时器
  	HC_SR04_Init();		//初始化超声波测距模块
		InfrarePSwitch_Init();
	
		LED0 = 0;
	  Buzzer_OFF();
#endif
	
	while (1)
	{

				len1=SimPro_UnPacking(SIMPRO1);	
		if(len1>0)//成功解析出数据
		{			
			if(len1==sizeof(struct UesrData)){//检查收到的串长度是否符合长度，避免数组越界
				recv_data_ptr1 = (struct UesrData*)SIMPRO1->data_buf;//指向解析缓冲区即可
				object_item=recv_data_ptr1->vari8;
			}		 
		}

		if(object_item!=10 && anti_trigger == 1){anti_trigger = 0;delay_flag[0] = 0;delay_flag[7] = 1;item_num--;object_item = 10;}//检测到物品在载物台，暂停8s,anti_trigger防止多次计数
		if(delay_flag[7] == 0&&item_num != 4){delay_flag[0] = 1;}//8s结束，启动传送带
		//if(item_num <= 0){delay_flag[0] = 0;object_item = 10;}//8s结束，启动传送带
		//if(delay_flag[7] == 0&&item_num <= 0){delay_flag[0] = 0;item_num = 4;}//最多3个物体传送，超过3个物品自动复位，等待超声波触发
		if(item_num < 0){delay_flag[0] = 0;item_num = 4;delay_flag[7] = 0;}//最多3个物体传送，超过3个物品自动复位，等待超声波触发

		
					
#if 1		
		Distance_mm[0]=sonar_mm2();			//获取距离测量结果，单位毫米（mm）
	//	Distance_mm[1]=sonar_mm();			//获取距离测量结果，单位毫米（mm）


		if(Distance_mm[0] <= 50&& Distance_mm[0] != 0){
			delay_flag[0] = 1;}
		if(delay_flag[0] == 1){
			 left_f(100);}
		if(delay_flag[0] == 0)left_f(0);
		OLED_ShowNum(4, 1, delay_flag[0], numlen(delay_flag[0]));
		
			 
//		if(Distance_mm[1] <= 50&& Distance_mm[1] != 0){
//			delay_flag[1] = 1;}
//		if(delay_flag[1] == 1){
//			 Push();}
//		if(delay_flag[0] == 0)Pull();
//		OLED_ShowNum(4, 3, delay_flag[1], numlen(delay_flag[1]));
			 
		//signal = InfrarePSwitch_Check();
		Full_Load_Warning(); 
			 
		OLED_ShowNum(3, 3, signal[0], numlen(signal[0]));
		OLED_ShowNum(3, 5, signal[1], numlen(signal[1]));
		OLED_ShowNum(3, 7, signal[2], numlen(signal[2]));
		OLED_ShowNum(3, 9, signal[3], numlen(signal[3]));
			 
		
			 if(oled_clear_flag == 1){
				 OLED_Clear();
				 oled_clear_flag = 0;}
				 
#endif		
				
			
		
	}
				
}

int16_t overload[4] ={0};
void Full_Load_Warning(void)
{
	signal = InfrarePSwitch_Check();
	if(signal[0] == 0){
		delay_flag[2] = 1;
		if(overload[0] == 1&&delay_flag[6] == 0){
		OLED_ShowString(1, 1, "other waste");
		Buzzer_ON();
		delay_flag[6] = 1;
		}
		else {OLED_ShowString(1, 1, ".");
			}
	}
	else delay_flag[2] = 0;
	
	if(signal[1] == 0){
		delay_flag[3] = 1;
		if(overload[1] == 1&&delay_flag[6] == 0){
		OLED_ShowString(1, 1, "kitchen waste");
		Buzzer_ON();
		delay_flag[6] = 1;
		}
		else OLED_ShowString(1, 1, ".");
	}
	else delay_flag[3] = 0;
	if(signal[2] == 0){
		delay_flag[4] = 1;
		if(overload[2] == 1&&delay_flag[6] == 0){
		OLED_ShowString(1, 1, "hazardous waste");
		Buzzer_ON();
		delay_flag[6] = 1;
		}
		else OLED_ShowString(1, 1, ".");
	}
	else delay_flag[4] = 0;
	if(signal[3] == 0){
		delay_flag[5] = 1;
		if(overload[3] == 1&&delay_flag[6] == 0){
		OLED_ShowString(1, 1, "Recyclable");
		Buzzer_ON();
		delay_flag[6] = 1;
		}
		else OLED_ShowString(1, 1, ".");
	}
	else delay_flag[5] = 0;
	
	
}

int16_t time_1ms[9] ={0};

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		 time_1ms[0]++;
		
     if(time_1ms[0]==10)//10ms采样
			{
			if(delay_flag[0] == 1){//传送带超声波
				time_1ms[1]++;
				if(time_1ms[1] == 2000){
				delay_flag[0] = 0 ;
				time_1ms[1] = 0;}
			}
		 if(delay_flag[7] == 1){//传送带8秒等待
				time_1ms[8]++;
			 anti_trigger = 0;
				if(time_1ms[8] == 500){
					anti_trigger = 1;
				delay_flag[7] = 0;
				time_1ms[8] = 0;}
			}
			
		 if(delay_flag[1] == 1){//推杆超声波
				time_1ms[2]++;
				if(time_1ms[2] == 2500){
				delay_flag[1] = 0 ;
				time_1ms[2] = 0;}
			}
		 if(delay_flag[2] == 1){//满载警告
				time_1ms[3]++;
				if(time_1ms[3] == 300){
				delay_flag[2] = 0 ;
					overload[0] = 1;
				time_1ms[3] = 0;}
			}
		 if(delay_flag[3] == 1){//满载警告2
				time_1ms[4]++;
				if(time_1ms[4] == 300){
				delay_flag[3] = 0 ;
					overload[1] = 1;
				time_1ms[4] = 0;}
			}
		 if(delay_flag[4] == 1){//满载警告3
				time_1ms[5]++;
				if(time_1ms[5] == 300){
				delay_flag[4] = 0 ;
					overload[2] = 1;
				time_1ms[5] = 0;}
			}
		 if(delay_flag[5] == 1){//满载警告4
				time_1ms[6]++;
				if(time_1ms[6] == 300){
				delay_flag[5] = 0 ;
					overload[3] = 1;
				time_1ms[6] = 0;}
			}
		 if(delay_flag[6] == 1){//蜂鸣器触发时间
				time_1ms[7]++;
				if(time_1ms[7] == 500){
				delay_flag[6] = 0 ;
				Buzzer_OFF();
					for(int i = 0;i<4;i++)
				overload[i] =0;
				time_1ms[7] = 0;
				oled_clear_flag = 1;}
			}
		time_1ms[0] = 0;	
			}
        
	}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

uint64_t numlen(uint64_t num)//计算数字的长度
{
    uint64_t len = 1;        // 初始长度为1
    for(; num > 9; ++len)    // 判断num是否大于9，否则长度+1
        num /= 10;	         // 使用除法进行运算，直到num小于1
    return len;              // 返回长度的值
}
void Show_distance(void){

	  Distance_m[0]=Distance_mm[0]/1000;	//转换为米（m）为单位，将整数部分放入Distance_m
		Distance_m_p[0]=Distance_mm[0]%1000;	//转换为米（m）为单位，将小数部分放入Distance_m_p
		OLED_Clear_Part(2,1,16);			//将OLDE屏第2行清屏
		OLED_ShowNum(2, 1,Distance_m[0],numlen(Distance_m[0]));	//显示测量结果的整数部分
		OLED_ShowChar(2, 1+numlen(Distance_m[0]), '.');		//显示小数点
		if(Distance_m_p[0]<100){								//判断是否小于100毫米
			OLED_ShowChar(2, 1+numlen(Distance_m[0])+1,'0');								//因为单位是米，所以小于10cm时要加0
			OLED_ShowNum(2, 1+numlen(Distance_m[0])+2,Distance_m_p[0],numlen(Distance_m_p[0]));	//显示测量结果的小数部分
			OLED_ShowChar(2, 1+numlen(Distance_m[0])+2+numlen(Distance_m_p[0]), 'm');			//显示单位
		}else																			// https://blog.zeruns.tech
		{
			OLED_ShowNum(2, 1+numlen(Distance_m[0])+1,Distance_m_p[0],numlen(Distance_m_p[0]));	//显示测量结果的小数部分
			OLED_ShowChar(2, 1+numlen(Distance_m[0])+1+numlen(Distance_m_p[0]), 'm');			//显示单位
		}
		OLED_Clear_Part(3,1,16);			//将OLDE屏第3行清屏
		OLED_ShowNum(3, 1,Distance_mm[0],numlen(Distance_mm[0]));		//显示单位为毫米的距离结果
		OLED_ShowString(3, 1 + numlen(Distance_mm[0]), "mm");
		Delay_ms(100);						//延时300毫秒
}
