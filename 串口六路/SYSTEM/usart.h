/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                STM32 USART串口模块
--文件名    : usart.h
--版本      : V1.1
--作者      : 周瑞鹏
--创建日期  : 2012/12/18
--修改日期  : 2012/12/18
--注明      :
**************************************************************************************************
--修改历史：1) 对printf()进行了retarget
            2) 增加了对uCOS-II的支持
*************************************************************************************************/
#ifndef _USART_H_
#define _USART_H_
#include "sys.h"
#include "xprintf.h"

#define USART_REC_LEN  200
#define EN_USART1_RX   1

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u16 USART_RX_STA;         		//接收状态标记

void uart_init(u32 bound);
void uart_sendchar(u8 ch);
u8 uart_getchar(void);
void uart_echo(void);

#endif

