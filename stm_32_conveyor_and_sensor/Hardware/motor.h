#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void left_f(uint16_t speed);
void left_b(uint16_t speed);
void right_f(uint16_t speed);
void right_b(uint16_t speed);
void Pull(void);
void Push(void);
void Stop(void);

#endif
