#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "SimpleProtocol.h"

struct UesrData{
	int8_t vari8;
	float varf32[4];
	int8_t vari8s[4];	
	uint32_t varu32;
	char varstr[20];
	
	//... //�û��ɼӻ����޸��������ݣ��ܳ���һ�㲻����125�Ļ�û�¡�
};
extern struct UesrData send_data;
extern struct UesrData *recv_data_ptr;
extern int8_t offset; //С��������켣��ƫ������������ƫ��������ƫ��ȡֵ-112~+112������ֵԽ��Խƫ������
extern uint32_t road;//ʶ�𵽵�·�ڣ�0��ֱ���䣬1ʮ��·�ڣ�2��ֱ���䣩	 //��ʱ������ʼ��	
extern int8_t number1;	
extern int8_t number2;	
extern int8_t location_of_num1;	
extern int8_t location_of_num2;	

u16 len=0;
void Get_Data_From_K210()
{
		len=SimPro_UnPacking(SIMPRO1);//��鲢���Խ�������
		if(len>0)//�ɹ�����������
		{			
				if(len==sizeof(struct UesrData)){//����յ��Ĵ������Ƿ���ϳ��ȣ���������Խ��
						recv_data_ptr = (struct UesrData*)SIMPRO1->data_buf;//ָ���������������
					offset = 	recv_data_ptr->vari8;
					number1 = 	recv_data_ptr->vari8s[0];
					location_of_num1= 	recv_data_ptr->vari8s[1];
					number2 = 	recv_data_ptr->vari8s[2];
					location_of_num2 = recv_data_ptr->vari8s[3];
					road = recv_data_ptr->varu32;
					
				}
		}
	}