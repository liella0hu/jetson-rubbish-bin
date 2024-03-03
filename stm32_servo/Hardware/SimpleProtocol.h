#ifndef SIMPLEPROTOCOL__H
#define SIMPLEPROTOCOL__H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
   生物电子创新基地
作者: 黄涛(865355983@qq.com)
版本: V1.0 
日期: 2023/06/07
    新一代通信协议,旨在为开发者提供更简易使用的接口,并带来更可靠的性能.
*/


//创建n个初始的全局变量
#define MAX_STATIC_SIMPRO 3  
// 缓冲区大小
#define SIMPRO_BUF_SIZE 256
// 缓冲区最多允许包含的包数量
#define SIMPRO_LOCAL_IDX_SIZE 32 

/*                        
                                协议结构图
    0                   1                   2                   3 
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
    | 填充非pad_flag |   head_flag   |    pad_flag   |    pack_idx   |
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

    uint8_t last_char; // 最后一个收到的数据，用于判别pad
    uint8_t start_unpacking; // 标记正在进行unpacking解析，禁止enchar修改recv_start

    int head_idx;
    int tail_idx;

    int recv_start; // start由解包控制
    int recv_end;   // end由接收控制

    // 存储接收到的数据
    uint8_t recv_buf[SIMPRO_BUF_SIZE];

    int count_reco_size; 
    // 存储识别到头的数据
    uint8_t reco_buf[SIMPRO_BUF_SIZE];

}SimpleProtocolContext;

typedef struct
{
    SimpleProtocolContext priv_data;
    uint8_t pack_idx; // 包序号，发送者每调用一次发送顺序递增,接受者可以计算出丢包情况

    int send_size; // 封包后的数据长度
    uint8_t send_buf[SIMPRO_BUF_SIZE];

    uint8_t data_pack_idx;      // 接收到的包序号
    int data_size;              // 解析后的数据长度
    uint8_t data_buf[SIMPRO_BUF_SIZE]; // 解析完成后的数据缓冲区
}SimpleProtocol;

extern SimpleProtocol __SIMPROS[MAX_STATIC_SIMPRO];
//创建指向SimpleProtocol的指针,方便用户使用
static SimpleProtocol *SIMPRO1 = & __SIMPROS[0]; 
static SimpleProtocol *SIMPRO2 = & __SIMPROS[1];
static SimpleProtocol *SIMPRO3 = & __SIMPROS[2];

//用户可用接口
void SimPro_Init(SimpleProtocol *SIMPRO);
void SimPro_InitAll(void);

int SimPro_Packet(SimpleProtocol *SIMPRO, void *buffer, int buf_size);
void SimPro_EnChar(SimpleProtocol *SIMPRO, uint8_t byte);
int SimPro_UnPacking(SimpleProtocol *SIMPRO);

#endif
