#ifndef _BSP_H_
#define _BSP_H_

#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "usmart.h"
#include "steer.h"
#include "key.h"
#include "timer.h"


//���С��ģʽת��
#define BYTE0(dwTemp)	(* ((char *)(&dwTemp) + 0) )
#define BYTE1(dwTemp)   (* ((char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)   (* ((char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)   (* ((char *)(&dwTemp) + 3) )

#define PI                 	(3.14159265358)
#define ANG_TO_RAD(angle)  	(angle*0.01745329251)//�Ƕ�ת���ȳ������ϵ��
#define RAD_TO_ANG(rad)    	(rad*57.2957795130)//����ת�Ƕ�

#define COUNTOF(a)	(sizeof(a)/sizeof(a[0]))

void hw_init(void);

#endif

