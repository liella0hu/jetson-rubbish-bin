/*
 * uart_for_encode.c
 *
 *  Created on: 2022年7月28日
 *      Author: hasee
 */
#include "uart_for_encode.h"

#define BUF_SIZE 100
/*
UartEncodeSendInit(0x01,0xAF,0xF1);//匿名上位机固定用户协议
while(1){
	//循环发送，最多六组数据
	uart_encode.pack1=(int32_t)(1);
	uart_encode.pack2=(int32_t)(2);
	uart_encode.pack3=(int32_t)(3);
	uart_encode.pack4=(int32_t)(4);


	uart_encode.pack5=(int32_t)(5);
	uart_encode.pack6=(int32_t)(6);

	UartSpeedEncode(&huart4);
}

*/
uint8_t TX_BUF[BUF_SIZE];

FlyHeadStr head={0};
FlyTailStr tail={0};

UartEncodeStr uart_encode={0};  //用户填写数据

int UaetEncodeSendFlag=1;
uint32_t head_size=0;
uint32_t tail_size=0;
uint32_t uart_encode_size=0;

int UartEncodeSendInit(uint8_t s_addr,uint8_t d_addr,uint8_t cmd){
    /*
     * s_addr : 0x01
     * d_addr : 0xAF
     */
    head.head_flag= 0XAA;
//    head.head_flag= 0xAAAA;
    head.s_addr=s_addr;
    head.d_addr=d_addr;
    head.cmd= cmd;
    head.data_size = sizeof(uart_encode);
    tail.val_sum=0x00;

    head_size=sizeof(head);
    tail_size=sizeof(tail);
    uart_encode_size = sizeof(uart_encode);
    return 0;
}
/*
 *
 * int32_t pack1;
    int32_t pack2;
    int32_t pack3;
    int32_t pack4;
    int32_t pack5;
    int32_t pack6;

 * */

#define PACK1  pack1
#define PACK2  pack2
#define PACK3  pack3
#define PACK4  pack4
#define PACK5  pack5
#define PACK6  pack6
int memcpy_uart_encode(uint8_t *buf,UartEncodeStr *uart_encode){
    uint32_t count = 0;
    buf[count++]=*((uint8_t*)(&uart_encode->PACK1) + 3);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK1) + 2);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK1) + 1);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK1) + 0);

    buf[count++]=*((uint8_t*)(&uart_encode->PACK2) + 3);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK2) + 2);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK2) + 1);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK2) + 0);

    buf[count++]=*((uint8_t*)(&uart_encode->PACK3) + 3);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK3) + 2);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK3) + 1);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK3) + 0);

    buf[count++]=*((uint8_t*)(&uart_encode->PACK4) + 3);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK4) + 2);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK4) + 1);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK4) + 0);

    buf[count++]=*((uint8_t*)(&uart_encode->PACK5) + 3);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK5) + 2);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK5) + 1);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK5) + 0);

    buf[count++]=*((uint8_t*)(&uart_encode->PACK6) + 3);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK6) + 2);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK6) + 1);
    buf[count++]=*((uint8_t*)(&uart_encode->PACK6) + 0);

    return 0;
}

//uint32_t UartSpeedEncode(UART_HandleTypeDef* huart){
uint32_t UartSpeedEncode(USART_TypeDef * huart){
    if(!UaetEncodeSendFlag)return 0;
//    static uint32_t send_count=0;
    uint8_t val_sum = 0;
    uint8_t *data_p;
    uint32_t i;
    uint32_t send_size=0;
    head.data_size = sizeof(uart_encode);

    tail.val_sum = val_sum;
    memcpy(TX_BUF,&head,head_size);
    send_size+=head_size;
    memcpy_uart_encode(TX_BUF+head_size,&uart_encode);
    send_size+=uart_encode_size;

    data_p = (uint8_t *)TX_BUF;
    for(i=0;i<send_size;++i){
        val_sum += data_p[i];
    }
    tail.val_sum = val_sum;
    memcpy(TX_BUF+head_size+uart_encode_size,&tail ,tail_size);
    send_size+=tail_size;
    i=0;
    data_p = (uint8_t*)TX_BUF;
		
		
		//HAL_UART_Transmit(huart,data_p,send_size,1000);
    for(i=0;i<send_size;++i){
       USART_SendData(huart, data_p[i]);    //发送数据，BYTE写入TDR
				while (USART_GetFlagStatus(huart, USART_FLAG_TXE) == RESET);//等待下一个标志位，标志位置1后会自动清零，
    }
//    UARTprintf("send count %u\n",send_count++);

    return send_size;
}

