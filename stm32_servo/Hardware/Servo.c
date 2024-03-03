#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init(void)
{
	PWM_Init();
}

void Servo_SetAngle1(float Angle1)
{
	PWM_SetCompare1(Angle1 / 270 * 2000 + 500);
}
void Servo_SetAngle2(float Angle2)
{
	PWM_SetCompare2(Angle2 / 180 * 2000 + 500);
}
//void Servo_SetAngle3(float Angle3)
//{
//	PWM_SetCompare3(Angle3 / 270 * 2000 + 500);
//}
//void Servo_SetAngle4(float Angle4)
//{
//	PWM_SetCompare4(Angle4 / 270 * 2000 + 500);
//}
