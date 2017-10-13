/*************************************************************************************************
                                  STM32 SysTick模块程序
--文件名    : delay.c
--版本      : V1.0
--作者      : 周瑞鹏
--创建日期  : 2013/2/14
--修改日期  : 2013/2/14
--注明      : 1) 这里把SysTick用作延时，可以提高延时精确度
              2) SysTick是向下计数, SysTick->VAL计到0时，又会从SysTick->LOAD中取出重装值，
                 再继续向下计数.
              3) 2013/4/2，使用uCOS-II系统时，可以调用delay_ms和delay_us来做任务延时，
                           也可以调用OSTimeDly()和OSTimeDlyHSMM()来做任务延时。
--Copyright(C) ZRP 2012-2022  All rights reserved
**************************************************************************************************
--修改历史：2013/4/2增加了对uCOS-II的支持，使用OS时，SysTick用作时钟节拍计数
              只要运行了uCOS-II,宏定义OS_VERSION就会被定义，所以用OS_VERSION作为预编译依据
*************************************************************************************************/
#include "delay.h"

#if  USE_uCOS_II_EN > 0u //如果使用的uCOS-II，就包含这个头文件
#include "includes.h"
#endif

static u8  fac_us=0;   //1us延时需要给SysTick->LOAD重装的数值
static u16 fac_ms=0;   //1ms延时需要给SysTick->LOAD重装的数值

#ifdef OS_VERSION
void SysTick_Handler(void)
{
    OSIntEnter();      //进入中断
    OSTimeTick();      //调用uCOS-II的滴答函数
    OSIntExit();       //退出中断
}
#endif
/*************************************************************************************************
--函数功能：初始化systick     (用户调用)
--参数列表：SYSCLK : 系统的时钟
--返 回 值：无
--注    明：1) SYSTICK的时钟固定为HCLK时钟的1/8，
            2) delay_init(72);
            3) OS_TICKS_PER_SEC就是 1s 内有多少次滴答
*************************************************************************************************/
void delay_init(u8 sysclk)
{
#ifdef OS_VERSION//如果运行了uCOS-II，SysTick就作为系统的时钟节拍
    u32 reload=0;
#endif
	SysTick->CTRL &= ~(1<<2);//bit2清空,选择外部时钟
	fac_us = sysclk/8;       //因为是HCLK/8，所以72MHz下9个SysTick为1us

#ifdef OS_VERSION
    reload=sysclk/8;         //reload此时就是1us的重装值，所以下面是*1000000，就是算1s
    reload=reload*1000000/OS_TICKS_PER_SEC;//uCOS-II的1s(1000000us), 一个时钟节拍是多长时间
                                           //就是要计算一个时钟节拍应该重装多大的数值
    fac_ms=1000/OS_TICKS_PER_SEC;//fac_ms = (1/OS_TICKS_PER_SEC) = OS最小的时长
	SysTick->CTRL|=1<<1;   	//开启SysTick中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=1<<0;   	//开启SYSTICK
#else
	fac_ms = (u16)fac_us*1000;//ms重装值的倍数,SysTick用作普通延时的时候,非uCOS-II的时候
#endif
}

#ifdef OS_VERSION//运行了uCOS-II，也为它提供两个非常精确的延时函数
/*************************************************************************************************
--函数功能：在uCOS-II系统中，延时nms   (用户调用)
--参数列表：nms      要延时的ms数
--返 回 值：无
--注    明：1) SysTick->LOAD为24位, 8个tick为1us, 所以最大延时为:nms<=(0xffffff/8)/1000
            2) nms/fac_ms,       大于OS最小时长的部分用OSTimeDly();
              (nms%fac_ms)*1000, 小于OS最小时长的部分用delay_us();
*************************************************************************************************/
void delay_ms(u16 nms)
{
    if(OSRunning==TRUE)
    {
        if(nms>=fac_ms)//如果延时大于OS的最小时间
        {
            OSTimeDly(nms/fac_ms);//nms/fac_ms,大于OS最小时长的部分用OSTimeDly();
        }
    }
    delay_us((u32)(nms%fac_ms)*1000);//(nms%fac_ms)*1000, 小于OS最小时长的部分用delay_us();
}

/*************************************************************************************************
--函数功能：在uCOS-II系统中，延时nus   (用户调用)
--参数列表：nus      要延时的us数
--返 回 值：无
--注    明：1) SysTick->LOAD为24位, 8个tick为1us, 所以最大延时为:nms<=(0xffffff/8)/1000
            2) 对72M条件下, nms<=2097ms, SYSCLK单位为Hz,nms单位为ms
*************************************************************************************************/
void delay_us(u32 nus)
{
    u32 ticks=0, tnow=0, told=0, tcnt=0;
    u32 reload=SysTick->LOAD;
    ticks=nus*fac_us;       //计算出本次延时多少个tick
    OSSchedLock();          //OS任务调度器上锁
    told=SysTick->VAL;      //保存旧值
    for(;;)
    {
        tnow=SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow<told)
                tcnt=tcnt+(told-tnow);//SysTick是向下计数的, 所以是told-tnow
            else tcnt+=reload+(told-tnow);
            told=tnow;          //told一直尾随着tnow
            if(tcnt>ticks)break;//计数达到ticks了
        }
    }
    OSSchedUnlock();        //解锁任务调度器
}
#else
//不使用uCOS-II
/*************************************************************************************************
--函数功能：延时nms   (用户调用)
--参数列表：nms      要延时的ms数
--返 回 值：无
--注    明：1) SysTick->LOAD为24位, 8个tick为1us, 所以最大延时为:nms<=(0xffffff/8)/1000
            2) 对72M条件下, nms<=2097ms, SYSCLK单位为Hz,nms单位为ms
*************************************************************************************************/
void delay_ms(u16 nms)
{
	u32 temp;

	SysTick->LOAD = (u32)nms*fac_ms;  //时间加载
	SysTick->VAL = 0x00;              //清空计数器
	SysTick->CTRL|= 1<<0;             //开启倒数
	do{
		temp = SysTick->CTRL;
	}
	while( temp&0x01 && !( temp&0x10000 ) );//等待时间到达
	SysTick->CTRL = 0x00;             //关闭计数器
	SysTick->VAL  = 0X00;             //清空计数器
}

/*************************************************************************************************
--函数功能：延时nus     (用户调用)
--参数列表：nus     要延时的us数
--返 回 值：无
--注    明：1) 72MHz下，8tick=1us, 0xFFFFFFtick=2 097 151.875us
            2) SysTick->LOAD为24位, 8个tick为1us
*************************************************************************************************/
void delay_us(u32 nus)
{
	u32 temp;

	SysTick->LOAD = nus*fac_us; //时间加载
	SysTick->VAL = 0x00;        //清空计数器
	SysTick->CTRL = 0x01 ;      //开始倒数
	do{
		temp = SysTick->CTRL;
	}
	while( temp&0x01 && !( temp&0x10000 ) );//等待时间到达
	SysTick->CTRL = 0x00;       //关闭计数器
	SysTick->VAL  = 0X00;       //清空计数器
}
#endif





