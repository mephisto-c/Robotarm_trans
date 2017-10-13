/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                舵机控制
--文件名    : steer.h
--版本      : V1.0
--作者      : 周瑞鹏
--创建日期  : 2013/3/18
--修改日期  : 2013/3/18
--注明      : 1) 舵机控制的PWM是 0.5ms~2.5ms, 20ms周期, 50Hz
                 舵机 steering engine
             _______ yn_______
            |        |        |
          xp|________|________|xn
            |        |        |
            |________|________|
                     yp
                        | MIN_PWM(yn)
                        |
                        |
   (xp)900 x<-----------|----------- 180(xn)
                        |
                        |
                       \|/y MAX_PWM(yp)
**************************************************************************************************
--修改历史：
*************************************************************************************************/
#include "steer.h"
#include "stm32f10x_lib.h"
#include "timer.h"

ENGINE SE[6]={
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
{0,0,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM,(MAX_PWM-MIN_PWM)*0.5+MIN_PWM, 1},
};

u32 SYS_CNT=0;//调度时钟
//1ms
void TIM3_IRQHandler(void)
{
	if(TIM3->SR & TIM_FLAG_Update)//更新中断
	{
		SYS_CNT++;//调度时钟
		steer_exe_int();
	}
	TIM3->SR &= ~TIM_FLAG_Update;
}
//定时器3定时中断
void tim3_int_init(u16 arr, u16 psc)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	RCC->APB1ENR |= RCC_APB1Periph_TIM3;

	TIM3->ARR = arr;
	TIM3->PSC = psc;
 	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
}

//定时器输出pwm波
void TIM4_PWM_Init(u16 arr, u16 psc)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC->APB2ENR |= RCC_APB2Periph_GPIOB;
	RCC->APB1ENR |= RCC_APB1Periph_TIM4;

	GPIOB->CRL &= 0X000FFFFF;//B6,B7
	GPIOB->CRL |= 0XBBB00000;//CH1
//	GPIOB->CRH &= 0XFFFFFFF0;//B8
//	GPIOB->CRH |= 0X0000000B;//CH3
	GPIOB->ODR |= 0X00E0;

	TIM4->ARR = arr;
	TIM4->PSC = psc;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = (arr+1)*0.3;
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;

	//channel 1
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM4, TIM_Channel_1, TIM_CCx_Enable);
	
	//channel 3
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM4, TIM_Channel_2, TIM_CCx_Enable);
	
//	TIM_ARRPreloadConfig(TIM4, ENABLE);

	TIM_Cmd(TIM4, ENABLE);
}

void TIM2_PWM_Init(u16 arr, u16 psc)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC->APB1ENR |= RCC_APB1Periph_TIM2;
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA;
	GPIOA->CRL &= 0xFFFF0000;
	GPIOA->CRL |= 0x0000BBBB;
    GPIOA->ODR |= 0X000F;
//	AFIO->MAPR|=3<<8;      //全部重映射

	TIM2->ARR = arr;
	TIM2->PSC = psc;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;//TIM1, TIM8才有的东西
	TIM_OCInitStructure.TIM_Pulse = (MAX_PWM-MIN_PWM)*0.5+MIN_PWM;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;//TIM1, TIM8才有的东西
//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;//TIM1, TIM8刹车功能用
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;//TIM1, TIM8刹车功能用

	//channel 1
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM2, TIM_Channel_1, TIM_CCx_Enable);

	//channel 2
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM2, TIM_Channel_2, TIM_CCx_Enable);

	//channel 3
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM2, TIM_Channel_3, TIM_CCx_Enable);

	//channel 4
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_CCxCmd(TIM2, TIM_Channel_4, TIM_CCx_Enable);

//	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}


//1ms
void steer_exe_int(void)
{
	static u16 cnt[6]={0,0,0,0,0,0,};
	u8 i;

	for (i=0; i<6; i++)
	{
		if (SE[i].sta==ENGINE_RUN)
		{
			if (SE[i].angle != SE[i].setangle)
			{
				cnt[i]++;
				if (cnt[i] >= SE[i].speed)
				{
					cnt[i]=0;

					switch(SE[i].dir)
					{
					case 0: SE[i].angle++;break;
					case 1: SE[i].angle--;break;
					}
					steer_set_pwm(SE[i].angle, i);//输出脉宽波
				}
			}
			else
			{
				SE[i].sta=ENGINE_STOP;
			}
		}
	}
}

//设置舵机控制PWM波
void steer_set_pwm(s16 pwm, char sel)
{
	if(sel==0)
		TIM2->CCR1=pwm;
	else if(sel==1)
		TIM2->CCR2=pwm;
	else if(sel==2)
		TIM2->CCR3=pwm;
	else if(sel==3)
		TIM2->CCR4=pwm;
	else if(sel==4)
		TIM4->CCR1=pwm;
	else if(sel==5)
		TIM4->CCR2=pwm;
}


//-------------------------用户调用函数---------------------
/*
设置一个舵机角度速度
angle : MIN_PWM  MAX_PWM
sel   : 0~5  6个舵机选择
speed : x ms  移动速度，数字大，移动慢，最快1ms
*/
void steer_set_sel(s16 angle, char sel, s16 speed)
{
	if (SE[sel].sta==ENGINE_STOP)
	{
		if (angle <= MIN_PWM)
			angle = MIN_PWM;
		if (angle >= MAX_PWM)
			angle = MAX_PWM;
		if ((SE[sel].angle-angle) >= 0)//判断舵机运行方向
		{
			SE[sel].dir = 1;
		}
		else
		{
			SE[sel].dir = 0;
		}
		SE[sel].setangle = angle;
		SE[sel].speed = speed;//速度
		SE[sel].sta=ENGINE_RUN;
	}
}

/*
设置所有舵机的角度和速度
ang0: 舵机0的角度, 取值范围180~900
...
speed : 所有舵机的移动速度
*/
void steer_set_all(s16 ang0, s16 ang1, s16 ang2, s16 ang3, s16 ang4, s16 ang5, s16 speed)
{
	steer_set_sel(ang0, 0, speed);
	steer_set_sel(ang1, 1, speed);
	steer_set_sel(ang2, 2, speed);
	steer_set_sel(ang3, 3, speed);
	steer_set_sel(ang4, 4, speed);
	steer_set_sel(ang5, 5, speed);
}

/*
舵机控制脚
A0 A1 A2 A3 B6 B8
*/
void steer_init(void)
{
	u8 i;
	tim3_int_init(99, 719);//1ms 定时中断
	TIM4_PWM_Init(7199, 199);
	TIM2_PWM_Init(7199, 199);//20ms 180~900
//	tim1_int_init(1,719);
//	TIM5_Int_Init(10,71);
	for(i=0; i<6; i++)//初始化PWM为一半，也就是舵机转到90度位置
	steer_set_pwm((MAX_PWM-MIN_PWM)*0.5+MIN_PWM, i);
}



