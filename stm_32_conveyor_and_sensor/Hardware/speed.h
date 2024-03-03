#ifndef __SPEED_H
#define __SPEED_H

float rotate_speed(signed short M , float N , float T);//m—T时间内测得的编码器脉冲个数；T—测速采样时间(ms）；N——编码器每转脉冲数。
float wheel_speed(float rotate ,signed short R);//Angular—当前角速度；R--轮子半径

float swerving(void);
float coincidence(void);  //y圆心重合
//float Amendment(float tar);
float Amendment(float linear_speed,float sensor_cent_size,int turn);//-1左，0中，1右
float getLeftSpeet(float linear_speed,float angle_speed);
float getRightSpeet(float linear_speed,float angle_speed);

#endif

