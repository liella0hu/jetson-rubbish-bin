#ifndef __PWM_H
#define __PWM_H

void TIM2_PWM_Init(u16 arr,u16 psc);
void TIM1_PWM_Init(u16 psc,u16 arr);

void PWM_SetCompare1(uint16_t Compare);
void PWM_SetCompare2(uint16_t Compare);
void PWM_SetPrescaler(uint16_t Prescaler);



#endif
