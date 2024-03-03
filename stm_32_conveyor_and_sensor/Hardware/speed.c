#include "math.h"
#include "sys.h"
#include "stm32f10x.h"  




float rotate_speed(signed short M , float N , float T)//m—T时间内测得的编码器脉冲个数；T—测速采样时间(ms）；N——编码器每转脉冲数。
{
		float n=0 ;
		//float m=M;
		n=(float)M/(N*T/1000);
		return n;
}

float wheel_speed(float rotate ,signed short R)//rotate—当前角速度；R--轮子半径mm
{
	float v=0;
	v=(float)rotate*2*R*3.1415926/1000;
	
	return v;

}


/*
*  
两个传感器之间的距离；mm 
*  car_leng    传感器到电机的距离；mm
*	 car_wid     轮子到车中间的距离；mm
*  tar         当前轮子的速度；m/s
*/


float getLeftSpeet(float linear_speed,float angle_speed){
	return linear_speed - angle_speed * car_wid / 1000.0 /2;
}
float getRightSpeet(float linear_speed,float angle_speed){
	return linear_speed + angle_speed * car_wid / 1000.0 /2;
}
//float cycle(void)
//{
//	car_wid*3.1415926/1000;
//}
int16_t positionc_encode1 =0,temp1 =0;
int16_t positionc_encode2 =0,temp2 =0;
float swerving()
{
	positionc_encode1 = car_wid/(Wheel_radius*2)*440;
	temp1 = positionc_encode1;
	positionc_encode1 = 0;
	return temp1 ;
}

float coincidence()  //y圆心重合
{
	 positionc_encode2 = car_leng/(Wheel_radius*3.1415926*2)*440;
	 temp2 = positionc_encode2;
	 positionc_encode2 = 0;
	 return temp2 ;
}

float cos_a=0;
float Amendment(float linear_speed,float sensor_cent_size,int turn)//-1左，0中，1右
{
		
	//float sensor_wid = 15
	//car_leng = 88
	//car_wid = 75;
		//turn_angle = atan(car_leng/sensor_wid)
		// turn_line = sqrt((sensor_wid*sensor_wid) + (car_leng * car_leng))
		// R = turn_line /2 * cos(turn_angle)
		
	//float left_speed,right_speed;
	//car_leng=200;
		//turn_angle = atan2(car_leng,sensor_wid);
		if(turn==0){//直线
			return 0;
		}
		turn_angle = atan(car_leng/sensor_cent_size);
		turn_line = sqrt((sensor_cent_size*sensor_cent_size) + (car_leng * car_leng));
	  cos_a=	cos(turn_angle);
		R2 = turn_line/1000.0 /2.0 / cos(turn_angle);
   		angle_speed_temp = linear_speed /R2;
		
		if(turn==-1)return angle_speed_temp;
		return -angle_speed_temp;
	
//		R = (sensor_wid * sensor_wid + car_leng * car_leng)/(sensor_wid*2)+car_wid;//计算半径；
//    Angle =180 - 2*(float)atan((double)(car_leng/sensor_wid))*180/3.1415926;//
//	
//		
//		Arc_length = Angle * R * 3.1415926/ 180;
//	
//		Amend_tar = Arc_length/(car_leng/linear_speed);//外面轮的速度
//	
//		return Amend_tar;
}

//			if(init_pid_error==1){
//				pid_init();
//				init_pid_error=0;
//				left_f(0);
//				right_f(0);
//				set_left_tar=0;
//				set_right_tar=0;
//				//set1=1;
//			}
