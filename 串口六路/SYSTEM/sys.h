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
                 其他的模块程序只要有涉及这些的，都要包含 #include"sys.h"
**************************************************************************************************
--修改历史：1) 2013/4/2增加了一个宏定义" #define USE_uCOS_II_EN  0u  "
               只要运行了uCOS-II,宏定义OS_VERSION就会被定义，所以用OS_VERSION作为预编译依据
*************************************************************************************************/
#ifndef _SYS_H_
#define _SYS_H_
#include "stm32f10x_lib.h"

#define USE_uCOS_II_EN   0u//是否使用uCOS-II, 0: 没使用  1:使用了

//位带操作,IO口操作宏定义
#define BITBAND(addr, bitnum)    ((addr&0xF0000000)+0x2000000+((addr&0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)           *((volatile unsigned long *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr,bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 

//单一IO口操作, n值0~15
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)
#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)
#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)

//JTAG模式
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00	

void RCC_Configuration(void);
void mco_out(void);
void jtag_set(u8 mode);
void WFI_SET(void);
void INTX_DISABLE(void);
void INTX_ENABLE(void);
void MSR_MSP(u32 addr);

#endif




