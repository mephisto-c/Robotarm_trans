/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                STM32 USART����ģ��
--�ļ���    : usart.h
--�汾      : V1.1
--����      : ������
--��������  : 2012/12/18
--�޸�����  : 2012/12/18
--ע��      :
**************************************************************************************************
--�޸���ʷ��1) ��printf()������retarget
            2) �����˶�uCOS-II��֧��
*************************************************************************************************/
#ifndef _USART_H_
#define _USART_H_
#include "sys.h"
#include "xprintf.h"

#define USART_REC_LEN  200
#define EN_USART1_RX   1

extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART_RX_STA;         		//����״̬���

void uart_init(u32 bound);
void uart_sendchar(u8 ch);
u8 uart_getchar(void);
void uart_echo(void);

#endif

