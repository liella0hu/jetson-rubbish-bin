#include "stm32f10x.h"                      // Device header
#include "pid.h"  
#include "sys.h"       
#include "speed.h" 
#include "motor.h"
#include "encoder.h"
#include "timer.h"
#include "led.h"
typedef struct PID PID;
struct PID{
	float Left_Err;
	float last_Left_Err;
	float next_Left_Err;
	float pwm;
	float add;
	float p;
	float i;
	float d;
	
};
PID left_pid;
PID right_pid;

void init_pid_struct(PID *pid){
	pid->Left_Err=0;
	pid->last_Left_Err=0;
	pid->next_Left_Err=0;
	pid->add=0;
	pid->pwm=0;
	
}

void pid_init(){
	init_pid_struct(&left_pid);
	init_pid_struct(&right_pid);
}

void init_set_pid(){
	left_pid.p=149;
	left_pid.i=9.32;
	left_pid.d=0.01;
	
	right_pid.p=149;
	right_pid.i=9.32;
	right_pid.d=0.01;
}

float myabs(float a)
{ 		   
	  float temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

void pwm_control(PID *pid)
{
    if(pid->pwm>100)
        pid->pwm=100;
    if(pid->pwm<0)
        pid->pwm=0;
}

float get_pid(PID *pid,float speed1,float tar1){
	speed1=myabs(speed1);
	pid->Left_Err=tar1-speed1;
	pid->add= pid->p * (pid->Left_Err - pid->last_Left_Err) + 
						pid->i * (pid->Left_Err) + 
						pid->d * (pid->Left_Err + pid->next_Left_Err - 2 * pid->last_Left_Err);
	pid->pwm+=pid->add;
	pwm_control(pid);
	pid->next_Left_Err=pid->last_Left_Err;
	pid->last_Left_Err=pid->Left_Err;
	return pid->pwm;
}
float Left_pid(float speed1,float tar1)
{
    return get_pid(&left_pid,speed1,tar1);
}


float Right_pid(float speed2,float tar2)
{
    return get_pid(&right_pid,speed2,tar2);
}

extern float Position_Kp, Position_Ki, Position_Kd; //���λ��PID���� 

int Position_PID_L(int Encoder, int Target)
{
                                       // ��� ��� ���� ��һ�����
    error_L = Target - Encoder;                                                                      // ����ٶ�ƫ��ɲ���ֵ��ȥĿ��ֵ��
    Integral_error_L += error_L; 
	 if(Integral_error_L> 20000) Integral_error_L= 20000;	//�����޷� ��ֹ����Ŀ��λ�ú����
	 if(Integral_error_L<-20000) Integral_error_L=-20000;	//�����޷� ��ֹ����Ŀ��λ�ú����	// ���ƫ��Ļ���
    out_L = Position_KP_L * error_L + Position_KI_L * Integral_error_L + Position_KD_L * (error_L - error_last_L); // λ��ʽPID������
    error_last_L = error_L;                                                                            // ������һ��ƫ��
    return out_L;                                                                                    // �������
}
int Position_PID_R(int Encoder, int Target)
{
                                              // ��� ��� ���� ��һ�����
    error_R = Target - Encoder;                                                                       // ����ٶ�ƫ��ɲ���ֵ��ȥĿ��ֵ��
    Integral_error_R += error_R;                                                                       // ���ƫ��Ļ���
	 if(Integral_error_R> 20000) Integral_error_R= 20000;	//�����޷� ��ֹ����Ŀ��λ�ú����
	  if(Integral_error_R<-20000) Integral_error_R=-20000;	//�����޷� ��ֹ����Ŀ��λ�ú����
    out_R = Position_KP_R * error_R + Position_KI_R * Integral_error_R + Position_KD_R * (error_R - error_last_R); // λ��ʽPID������
    error_last_R = error_R;                                                                            // ������һ��ƫ��
    return out_R;                                                                                    // �������
}


//int16_t time_1ms =0;
//int64_t Right_Position =0;
//int64_t Left_Position =0;
//int8_t flag=1;
//void TIM2_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//	{
//        time_1ms++;
//        if(time_1ms==10)//5ms����
//        {
//					
////			LED0=0;
////      Delay_ms(50);
////      LED0=1;
////      Delay_ms(50);
//					if(pid_flag==0) 
//					right_encode_num=Encoder2_get();
//					left_encode_num =Encoder1_get();
//	
//					Lspeed=wheel_speed( rotate_speed(left_encode_num ,  440 , time_1ms) ,35);//time_1ms = 5ms����
//					
//					Rspeed=wheel_speed( rotate_speed(right_encode_num , 440 , time_1ms) ,35);//time_1ms = 5ms����

//					if(set2&&(Right_Position-coincidence()-swerving()<=0||Left_Position-coincidence()<=0))	
//					{ set1 = 0;
////						right_f(0);						
////						left_f(0);	
//						Right_Position+=right_encode_num;		//����λ��	
//						Left_Position+=left_encode_num;		//����λ��				

//					//	Amendment(0.1,car_wid,-1);
//					//	getRightSpeet(0.1,Amendment(0.1,car_wid,-1););
////					R_pwm = Right_pid(Rspeed,getRightSpeet(0.1,Amendment(0.1,car_wid,-1)));
////					L_pwm = Left_pid(Lspeed,getLeftSpeet(0.1,Amendment(0.1,car_wid,-1)));

//						
//					L_pwm = Left_pid(Lspeed,0.1);
//					R_pwm = Right_pid(Rspeed,0.1);
//					
//				right_f(R_pwm);				
//				left_f(L_pwm);
//						
//						
////				right_f(Position_PID_R(Right_Position,coincidence()));						
////				left_f(Position_PID_L(Left_Position,coincidence()));	
//         if(Right_Position-coincidence()-swerving()>0||Left_Position-coincidence()>0)	
//					 {set2 =0;	right_f(0);						
//						left_f(0);	set1 = 0;	}
////				if(Left_Position-coincidence()>0)					
////					{set2 =0;	right_f(0);						
////						left_f(0);	set1 = 0;	}							
//						}				
//						pid_flag=1;			
//						time_1ms=0;
//							  
//        }
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//	}
//}
