#include "stm32f10x.h"
#include "Delay.h"

/*
具体使用说明请到我博客：// https://blog.zeruns.tech
*/

#define Echo GPIO_Pin_6		//HC-SR04模块的Echo脚 接GPIOB6
#define Trig GPIO_Pin_5		//HC-SR04模块的Trig脚接GPIOB5

#define Echo2 GPIO_Pin_7		//HC-SR04模块的Echo脚 接GPIOB6
#define Trig2 GPIO_Pin_6		//HC-SR04模块的Trig脚接GPIOB5

uint64_t time[2]={0};			//声明变量，用来计时
uint64_t time_end[2]={0};		//声明变量，存储回波信号时间

void HC_SR04_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA,ENABLE);	//启用GPIOB的外设时钟	
	GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//设置GPIO口为推挽输出
//	GPIO_InitStructure.GPIO_Pin = Trig  ;						//设置GPIO口5
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//设置GPIO口速度50Mhz
//	GPIO_Init(GPIOB,&GPIO_InitStructure);					//初始化GPIOB
		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//设置GPIO口为推挽输出
	GPIO_InitStructure.GPIO_Pin = Trig2 ;						//设置GPIO口5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//设置GPIO口速度50Mhz
	GPIO_Init(GPIOA,&GPIO_InitStructure);		
	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;			//设置GPIO口为下拉输入模式
//	GPIO_InitStructure.GPIO_Pin = Echo  ;						//设置GPIO口6
//	GPIO_Init(GPIOB,&GPIO_InitStructure);					//初始化GPIOB
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;			//设置GPIO口为下拉输入模式
	GPIO_InitStructure.GPIO_Pin =  Echo2 ;						//设置GPIO口6
	GPIO_Init(GPIOA,&GPIO_InitStructure);					//初始化GPIOB
//	GPIO_WriteBit(GPIOB,Trig,0);						//输出低电平
	GPIO_WriteBit(GPIOA,Trig2,0);						//输出低电平
	Delay_us(15);											//延时15微秒
}

int16_t sonar_mm(void)									//测距并返回单位为毫米的距离结果
{
	uint32_t Distance,Distance_mm = 0;
	GPIO_WriteBit(GPIOB,Trig,1);						//输出高电平
	Delay_us(15);										//延时15微秒
	GPIO_WriteBit(GPIOB,Trig,0);						//输出低电平
	while(GPIO_ReadInputDataBit(GPIOB,Echo)==0);		//等待低电平结束
	time[0]=0;												//计时清零
	while(GPIO_ReadInputDataBit(GPIOB,Echo)==1);		//等待高电平结束
	time_end[0]=time[0];										//记录结束时的时间
	if(time_end[0]/100<38)									//判断是否小于38毫秒，大于38毫秒的就是超时，直接调到下面返回0
	{
		Distance=(time_end[0]*346)/2;						//计算距离，25°C空气中的音速为346m/s
		Distance_mm=Distance/100;						//因为上面的time_end的单位是10微秒，所以要得出单位为毫米的距离结果，还得除以100
	}
	return Distance_mm;									//返回测距结果
}
int16_t sonar_mm2(void)									//测距并返回单位为毫米的距离结果
{
	uint32_t Distance,Distance_mm = 0;
	GPIO_WriteBit(GPIOA,Trig2,1);						//输出高电平
	Delay_us(15);										//延时15微秒
	GPIO_WriteBit(GPIOA,Trig2,0);						//输出低电平
	while(GPIO_ReadInputDataBit(GPIOA,Echo2)==0);		//等待低电平结束
	time[1]=0;												//计时清零
	while(GPIO_ReadInputDataBit(GPIOA,Echo2)==1);		//等待高电平结束
	time_end[1]=time[1];										//记录结束时的时间
	if(time_end[1]/100<38)									//判断是否小于38毫秒，大于38毫秒的就是超时，直接调到下面返回0
	{
		Distance=(time_end[1]*346)/2;						//计算距离，25°C空气中的音速为346m/s
		Distance_mm=Distance/100;						//因为上面的time_end的单位是10微秒，所以要得出单位为毫米的距离结果，还得除以100
	}
	return Distance_mm;									//返回测距结果
}

//float sonar(void)										//测距并返回单位为米的距离结果
//{
//	uint32_t Distance,Distance_mm = 0;
//	float Distance_m=0;
//	GPIO_WriteBit(GPIOB,Trig,1);					//输出高电平
//	Delay_us(15);
//	GPIO_WriteBit(GPIOB,Trig,0);					//输出低电平
//	while(GPIO_ReadInputDataBit(GPIOB,Echo)==0);
//	time=0;
//	while(GPIO_ReadInputDataBit(GPIOB,Echo)==1);
//	time_end=time;
//	if(time_end/100<38)
//	{
//		Distance=(time_end*346)/2;
//		Distance_mm=Distance/100;
//		Distance_m=Distance_mm/1000;
//	}
//	return Distance_m;
//}

void TIM3_IRQHandler(void)			//更新中断函数，用来计时，每10微秒变量time加1
{									// https://blog.zeruns.tech
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)		//获取TIM3定时器的更新中断标志位
	{
		(time[0])++;
		(time[1])++;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);			//清除更新中断标志位
	}
}
