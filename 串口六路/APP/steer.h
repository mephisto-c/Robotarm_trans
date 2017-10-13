#ifndef _STEER_H_
#define _STEER_H_
#include "sys.h"
#include "bsp.h" 

//舵机的脉宽比较最大最小值
#define MIN_PWM 180
#define MAX_PWM 900
#define ANGLE_RESOLUTION ((MAX_PWM-MIN_PWM)/180.0)

typedef struct e
{
	char sta;//舵机状态
	char dir;
	s16 angle;//当前的角度
	s16 setangle;//设置的角度
	s16 speed;//舵机运动速度
}ENGINE;
extern ENGINE SE[];
//舵机状态
#define ENGINE_RUN     1//舵机正在移动
#define ENGINE_STOP    0

void steer_exe_int(void);
void steer_set_pwm(s16 pwm, char sel);

void steer_init(void);
void steer_set_sel(s16 angx, char sel, s16 speed);
void steer_set_all(s16 ang0, s16 ang1, s16 ang2, s16 ang3, s16 ang4, s16 ang5, s16 speed);

extern u32 SYS_CNT;
#endif
