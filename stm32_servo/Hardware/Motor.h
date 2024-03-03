#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void Motor_SetSpeed(int Speed);
void Motor_Control(int TIM2_CCR);

#endif