#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "SimpleProtocol.h"

struct UesrData{
	int8_t vari8;
	float varf32[4];
	int8_t vari8s[4];	
	uint32_t varu32;
	char varstr[20];
	
	//... //用户可加或者修改其他数据，总长度一般不超过125的话没事。
};
extern struct UesrData send_data;
extern struct UesrData *recv_data_ptr;
extern int8_t offset; //小车车身与轨迹的偏移量（负数左偏，正数右偏，取值-112~+112，绝对值越大越偏离轨道）
extern uint32_t road;//识别到的路口（0左直角弯，1十字路口，2右直角弯）	 //延时函数初始化	
extern int8_t number1;	
extern int8_t number2;	
extern int8_t location_of_num1;	
extern int8_t location_of_num2;	

u16 len=0;
void Get_Data_From_K210()
{
		len=SimPro_UnPacking(SIMPRO1);//检查并尝试解析数据
		if(len>0)//成功解析出数据
		{			
				if(len==sizeof(struct UesrData)){//检查收到的串长度是否符合长度，避免数组越界
						recv_data_ptr = (struct UesrData*)SIMPRO1->data_buf;//指向解析缓冲区即可
					offset = 	recv_data_ptr->vari8;
					number1 = 	recv_data_ptr->vari8s[0];
					location_of_num1= 	recv_data_ptr->vari8s[1];
					number2 = 	recv_data_ptr->vari8s[2];
					location_of_num2 = recv_data_ptr->vari8s[3];
					road = recv_data_ptr->varu32;
					
				}
		}
	}