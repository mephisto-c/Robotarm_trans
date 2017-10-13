/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                STM32 system核心模块
--文件名    : sys.h
--版本      : V1.0
--作者      : 周瑞鹏
--创建日期  : 2012/12/18
--修改日期  : 2012/12/18
--注明      : 1) 本模块是STM32的核心程序，包含了内存地址映射，NVIC向量控制器，MDK的类型定义，
                 exti外部中断等, 其他的模块程序只要有涉及这些的，都要包含 #include"sys.h"
**************************************************************************************************
--修改历史：1) 2013/4/2增加了一个宏定义" #define USE_uCOS_II_EN  0u  "
               只要运行了uCOS-II,宏定义OS_VERSION就会被定义，所以用OS_VERSION作为预编译依据
            2) 增加void INTX_DISABLE(void)和void INTX_ENABLE(void)两个函数
               类似于51的EA=1，EA=0
*************************************************************************************************/
#include "sys.h"

void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);//72MHz

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);//72MHz

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);//36MHz

    /* ADCCLK = PCLK2/4 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);//16MHz

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field.
                                  This value must be a multiple of 0x200. */
#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, VECT_TAB_OFFSET);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, VECT_TAB_OFFSET);
#endif
  }
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//组2
}

//时钟输出
void mco_out(void)
{
	RCC->APB2ENR |= RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA;

    GPIOA->CRH &= 0xFFFFFFF0;
    GPIOA->CRH |= 0x0000000B;//PA8
    GPIOA->ODR |= 0x0100;
    RCC->CFGR &=0xF0FFFFFF;
    RCC->CFGR |=0x05000000; //4:sysclk输出   72MHz
                            //5:内部8MHz的RC 8MHz
                            //6:外部4~25MHz  8MHz
                            //7:PLL时钟2分频 36MHz
}

/*************************************************************************************************
--函数功能：JTAG模式设置      (用户调用)
--参数列表：mode :  00, JTAG_SWD_ENABLE;  01,SWD_ENABLE;  10,JTAG_SWD_DISABLE;
--返 回 值：无
--注    明：1) JTAG_Set(JTAG_SWD_ENABLE);
*************************************************************************************************/
void jtag_set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR |= RCC_APB2Periph_AFIO;     //开启辅助时钟
	AFIO->MAPR &= 0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR |= temp;       //设置jtag模式
}

/*************************************************************************************************
--函数功能：wait for interrupt汇编指令  (用户调用)
--参数列表：无
--返 回 值：无
--注    明：1) THUMB指令不支持汇编内联，采用如下方法实现执行汇编指令WFI
*************************************************************************************************/
__asm void WFI_SET(void)
{
	WFI;
}

/*************************************************************************************************
--函数功能：关闭所有中断  (用户调用)
--参数列表：无
--返 回 值：无
--注    明：1) 汇编内联
*************************************************************************************************/
__asm void INTX_DISABLE(void)
{
	CPSID I;
}

/*************************************************************************************************
--函数功能：开启所有中断  (用户调用)
--参数列表：无
--返 回 值：无
--注    明：1) 汇编内联
*************************************************************************************************/
__asm void INTX_ENABLE(void)
{
	CPSIE I;
}

/*************************************************************************************************
--函数功能：设置栈顶地址 (用户调用)
--参数列表：addr:栈顶地址
--返 回 值：无
--注    明：1) 汇编内联
*************************************************************************************************/
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 //set Main Stack value
    BX r14
}

