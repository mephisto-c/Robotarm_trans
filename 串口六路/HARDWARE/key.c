#include "stm32f10x_lib.h"
#include "key.h"

//#include "delay.h"
//#include "timer.c"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  


								    
//按键初始化函数
//void KEY_Init(void) //IO初始化
//{ 
// 	GPIO_InitTypeDef GPIO_InitStructure;
// 
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,PORTE时钟

//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//KEY0-KEY2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
// 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4

//	//初始化 WK_UP-->GPIOA.0	  下拉输入
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

//}




//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
//u16 led0pwmval=450;
u16 arr = 399;



void PWM_UP(void)
{
	if(arr > 10)
	{
		arr-=10;
//		arr++;
//		led0pwmval = arr / 2;
//		arr--;
		TIM5_Int_Init(arr,179);
	}	
}
void PWM_DOWN(void)
{
	if(arr < 899)
	{
		arr+=10;
//		arr = arr +1;
//		led0pwmval = arr / 2;
//		arr--;
		TIM5_Int_Init(arr,179);
	}	
}
void PE5_Setbit(void)
{
	LED1 = 1; 
	
}

void PE5_Resetbit(void)
{
	LED1 = 0; 
	
}

