#ifndef __ENCODER_H
#define __ENCODER_H


void Encoder1_Init_TIM3(void);
void Encoder2_Init_TIM4(void);

uint16_t Encoder2_get(void);
uint16_t Encoder1_get(void);
#endif
