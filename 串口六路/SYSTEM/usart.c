/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                STM32 USART串口模块
--文件名    : usart.c
--版本      : V1.1
--作者      : 周瑞鹏
--创建日期  : 2012/12/18
--修改日期  : 2012/12/18
--注明      :
**************************************************************************************************
--修改历史：1) 对printf()进行了retarget
            2) 增加了对uCOS-II的支持
*************************************************************************************************/
#include "usart.h"
#if  USE_uCOS_II_EN > 0u //如果使用的uCOS-II，就包含这个头文件
#include "includes.h"
#endif
#include "xprintf.h"

#if EN_USART1_RX   //如果使能了接收
u8 USART_RX_BUF[USART_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;           //接收状态标记
/*************************************************************************************************
--函数功能：串口1中断服务程序    (硬件调用)
--参数列表：无
--返 回 值：无
--注    明：1) 注意,读取USARTx->SR能避免莫名其妙的错误
            2)  回车符 = \r\n  0x0d 0x0a, \r\n并没有接在接收缓冲后面
*************************************************************************************************/
void USART1_IRQHandler(void)
{
	u8 res;
#if  USE_uCOS_II_EN > 0u //如果使用的uCOS-II
	OSIntEnter();
#endif
	if(USART1->SR&(1<<5))//接收到数据
	{
		res=USART1->DR;
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了 \r
			{
				if(res!='\n')
                    USART_RX_STA=0;//接收错误,重新开始
				else
                    USART_RX_STA|=0x8000;	//接收完成了
			}
            else //还没收到'\r'
			{
				if(res=='\r')
                {
                    USART_RX_STA|=0x4000;
                }
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
                        USART_RX_STA=0;//接收数据错误,重新开始接收
				}
			}
		}
	}
#if  USE_uCOS_II_EN > 0u //如果使用的uCOS-II
	OSIntExit();
#endif
}
#endif

//串口回声
void uart_echo(void)
{
	int i;
	if(USART_RX_STA&0x8000)
	{
		USART_RX_STA=0;
		for(i=0; i<USART_REC_LEN; i++)
		{
			if((USART_RX_BUF[i]=='\r') || (USART_RX_BUF[i]=='\n'))
				break;
			uart_sendchar(USART_RX_BUF[i]);
		}
		xprintf("\r\n");
	}
}

void uart_sendchar(u8 ch)
{
	while((USART1->SR&USART_FLAG_TC)==0);//等待发送完成
	USART1->DR = ch&0x01FF;
}

u8 uart_getchar(void)
{
	while ((USART1->SR&USART_FLAG_RXNE)==0);//等待接收数据
	return (USART1->DR&0x01ff);
}

void uart_init(u32 baud)
{
    USART_InitTypeDef USART_InitStructure;
#if EN_USART1_RX		  //如果使能了接收
    NVIC_InitTypeDef   NVIC_InitStructure;
#endif
    /* USARTx configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    RCC->APB2ENR|= RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA;
    GPIOA->CRH &= 0xFFFFF00F;//TX  RX
    GPIOA->CRH |= 0x000008B0;//A10 A9
    GPIOA->ODR |= 0x0f00;

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure the USARTx */
    USART_Init(USART1, &USART_InitStructure);
    /* Enable the USARTx */
    USART_Cmd(USART1, ENABLE);
#if EN_USART1_RX		  //如果使能了接收
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//接收非空中断

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

    xfunc_out = uart_sendchar;
//	xfunc_in = uart_getchar;
}

