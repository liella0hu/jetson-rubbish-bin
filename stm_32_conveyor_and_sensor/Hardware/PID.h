#ifndef __PID_H
#define __PID_H

float Left_pid(float speed1,float tar1);
float Right_pid(float speed2,float tar2);
void pid_init(void);
void init_set_pid(void);
int Position_FeedbackControl(float Circle, int CurrentPosition);

extern uint16_t set1;



#endif
