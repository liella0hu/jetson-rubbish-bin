#ifndef __SPEED_H
#define __SPEED_H

float rotate_speed(signed short M , float N , float T);//m��Tʱ���ڲ�õı��������������T�����ٲ���ʱ��(ms����N����������ÿת��������
float wheel_speed(float rotate ,signed short R);//Angular����ǰ���ٶȣ�R--���Ӱ뾶

float swerving(void);
float coincidence(void);  //yԲ���غ�
//float Amendment(float tar);
float Amendment(float linear_speed,float sensor_cent_size,int turn);//-1��0�У�1��
float getLeftSpeet(float linear_speed,float angle_speed);
float getRightSpeet(float linear_speed,float angle_speed);

#endif

