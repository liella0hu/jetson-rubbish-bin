/*
 * uart_for_encode.h
 *
 *  Created on: 2022Äê7ÔÂ28ÈÕ
 *      Author: hasee
 */

#ifndef UART_FOR_ENCODE_H_
#define UART_FOR_ENCODE_H_

#include <string.h>
#include <stdint.h>
#include "stm32f10x.h"  

typedef struct FlyHead FlyHeadStr;
typedef struct FlyTail FlyTailStr;
typedef struct UartEncode UartEncodeStr;
struct FlyHead{
    uint8_t head_flag;
//    uint16_t head_flag;
    uint8_t s_addr;
    uint8_t d_addr;
    uint8_t cmd;
    uint8_t data_size;
};
struct FlyTail{
    uint8_t val_sum;
};
struct UartEncode{
    int32_t pack1;
    int32_t pack2;
    int32_t pack3;
    int32_t pack4;
    int32_t pack5;
    int32_t pack6;
};

extern UartEncodeStr uart_encode;

int UartEncodeSendInit(uint8_t s_addr,uint8_t d_addr,uint8_t cmd);
//uint32_t UartSpeedEncode(UART_HandleTypeDef *huart);
uint32_t UartSpeedEncode(USART_TypeDef *huart);
#endif /* UART_FOR_ENCODE_H_ */
