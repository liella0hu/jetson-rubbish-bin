#ifndef __GET_DATA_FROM_USART_H
#define __GET_DATA_FROM_USART_H

void Get_Data_From_K210();
struct UesrData{
	int8_t vari8;
	float varf32[4];
	int8_t vari8s[4];	
	uint32_t varu32;
	char varstr[20];
	
	//... //用户可加或者修改其他数据，总长度一般不超过125的话没事。
};
struct UesrData send_data;
struct UesrData *recv_data_ptr;
int8_t offset; //小车车身与轨迹的偏移量（负数左偏，正数右偏，取值-112~+112，绝对值越大越偏离轨道）
uint32_t road;//识别到的路口（0左直角弯，1十字路口，2右直角弯）	 //延时函数初始化	
int8_t number1;	
int8_t number2;	
int8_t location_of_num1;	
int8_t location_of_num2;	

#endif
