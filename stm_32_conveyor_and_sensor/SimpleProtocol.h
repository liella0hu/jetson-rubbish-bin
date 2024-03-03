#ifndef SIMPLEPROTOCOL__H
#define SIMPLEPROTOCOL__H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
   ������Ӵ��»���
����: ����(865355983@qq.com)
�汾: V1.0 
����: 2023/06/07
    ��һ��ͨ��Э��,ּ��Ϊ�������ṩ������ʹ�õĽӿ�,���������ɿ�������.
*/


//����n����ʼ��ȫ�ֱ���
#define MAX_STATIC_SIMPRO 3  
// ��������С
#define SIMPRO_BUF_SIZE 256
// �����������������İ�����
#define SIMPRO_LOCAL_IDX_SIZE 32 

/*                        
                                Э��ṹͼ
    0                   1                   2                   3 
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
    | ����pad_flag |   head_flag   |    pad_flag   |    pack_idx   |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |   pad_flag    |   data_size   |    pad_flag   |    val_sum    |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
    |                          user_data                            | 
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
    |                     ...                       |   tail_flags  | 
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  
*/
#ifndef uint8_t
typedef unsigned char uint8_t;
#endif
#ifndef int16_t
typedef short int16_t;
#endif
#ifndef uint16_t
typedef unsigned short uint16_t;
#endif
#ifndef int32_t
typedef int int32_t;
#endif
#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

typedef struct
{

    uint8_t last_char; // ���һ���յ������ݣ������б�pad
    uint8_t start_unpacking; // ������ڽ���unpacking��������ֹenchar�޸�recv_start

    int head_idx;
    int tail_idx;

    int recv_start; // start�ɽ������
    int recv_end;   // end�ɽ��տ���

    // �洢���յ�������
    uint8_t recv_buf[SIMPRO_BUF_SIZE];

    int count_reco_size; 
    // �洢ʶ��ͷ������
    uint8_t reco_buf[SIMPRO_BUF_SIZE];

}SimpleProtocolContext;

typedef struct
{
    SimpleProtocolContext priv_data;
    uint8_t pack_idx; // ����ţ�������ÿ����һ�η���˳�����,�����߿��Լ�����������

    int send_size; // ���������ݳ���
    uint8_t send_buf[SIMPRO_BUF_SIZE];

    uint8_t data_pack_idx;      // ���յ��İ����
    int data_size;              // ����������ݳ���
    uint8_t data_buf[SIMPRO_BUF_SIZE]; // ������ɺ�����ݻ�����
}SimpleProtocol;

extern SimpleProtocol __SIMPROS[MAX_STATIC_SIMPRO];
//����ָ��SimpleProtocol��ָ��,�����û�ʹ��
static SimpleProtocol *SIMPRO1 = & __SIMPROS[0]; 
static SimpleProtocol *SIMPRO2 = & __SIMPROS[1];
static SimpleProtocol *SIMPRO3 = & __SIMPROS[2];

//�û����ýӿ�
void SimPro_Init(SimpleProtocol *SIMPRO);
void SimPro_InitAll(void);

int SimPro_Packet(SimpleProtocol *SIMPRO, void *buffer, int buf_size);//���룬��װЭ��
void SimPro_EnChar(SimpleProtocol *SIMPRO, uint8_t byte);//��һ���ֽڽ���Э�黺����
int SimPro_UnPacking(SimpleProtocol *SIMPRO);//���Խ���Э��

#endif
