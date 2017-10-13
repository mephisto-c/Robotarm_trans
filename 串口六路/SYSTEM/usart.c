/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                STM32 USART����ģ��
--�ļ���    : usart.c
--�汾      : V1.1
--����      : ������
--��������  : 2012/12/18
--�޸�����  : 2012/12/18
--ע��      :
**************************************************************************************************
--�޸���ʷ��1) ��printf()������retarget
            2) �����˶�uCOS-II��֧��
*************************************************************************************************/
#include "usart.h"
#if  USE_uCOS_II_EN > 0u //���ʹ�õ�uCOS-II���Ͱ������ͷ�ļ�
#include "includes.h"
#endif
#include "xprintf.h"

#if EN_USART1_RX   //���ʹ���˽���
u8 USART_RX_BUF[USART_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;           //����״̬���
/*************************************************************************************************
--�������ܣ�����1�жϷ������    (Ӳ������)
--�����б���
--�� �� ֵ����
--ע    ����1) ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
            2)  �س��� = \r\n  0x0d 0x0a, \r\n��û�н��ڽ��ջ������
*************************************************************************************************/
void USART1_IRQHandler(void)
{
	u8 res;
#if  USE_uCOS_II_EN > 0u //���ʹ�õ�uCOS-II
	OSIntEnter();
#endif
	if(USART1->SR&(1<<5))//���յ�����
	{
		res=USART1->DR;
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ��� \r
			{
				if(res!='\n')
                    USART_RX_STA=0;//���մ���,���¿�ʼ
				else
                    USART_RX_STA|=0x8000;	//���������
			}
            else //��û�յ�'\r'
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
                        USART_RX_STA=0;//�������ݴ���,���¿�ʼ����
				}
			}
		}
	}
#if  USE_uCOS_II_EN > 0u //���ʹ�õ�uCOS-II
	OSIntExit();
#endif
}
#endif

//���ڻ���
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
	while((USART1->SR&USART_FLAG_TC)==0);//�ȴ��������
	USART1->DR = ch&0x01FF;
}

u8 uart_getchar(void)
{
	while ((USART1->SR&USART_FLAG_RXNE)==0);//�ȴ���������
	return (USART1->DR&0x01ff);
}

void uart_init(u32 baud)
{
    USART_InitTypeDef USART_InitStructure;
#if EN_USART1_RX		  //���ʹ���˽���
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
#if EN_USART1_RX		  //���ʹ���˽���
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���շǿ��ж�

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

    xfunc_out = uart_sendchar;
//	xfunc_in = uart_getchar;
}

