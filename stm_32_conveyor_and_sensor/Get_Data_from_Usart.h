#ifndef __GET_DATA_FROM_USART_H
#define __GET_DATA_FROM_USART_H

void Get_Data_From_K210();
struct UesrData{
	int8_t vari8;
	float varf32[4];
	int8_t vari8s[4];	
	uint32_t varu32;
	char varstr[20];
	
	//... //�û��ɼӻ����޸��������ݣ��ܳ���һ�㲻����125�Ļ�û�¡�
};
struct UesrData send_data;
struct UesrData *recv_data_ptr;
int8_t offset; //С��������켣��ƫ������������ƫ��������ƫ��ȡֵ-112~+112������ֵԽ��Խƫ������
uint32_t road;//ʶ�𵽵�·�ڣ�0��ֱ���䣬1ʮ��·�ڣ�2��ֱ���䣩	 //��ʱ������ʼ��	
int8_t number1;	
int8_t number2;	
int8_t location_of_num1;	
int8_t location_of_num2;	

#endif
