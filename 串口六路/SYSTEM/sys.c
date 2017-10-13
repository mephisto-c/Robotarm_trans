/*************************************************************************************************
                Copyright (C) 2012-2022, Ray Chou, Innovation Laboratory
                              All rights reserved

                                STM32 system����ģ��
--�ļ���    : sys.h
--�汾      : V1.0
--����      : ������
--��������  : 2012/12/18
--�޸�����  : 2012/12/18
--ע��      : 1) ��ģ����STM32�ĺ��ĳ��򣬰������ڴ��ַӳ�䣬NVIC������������MDK�����Ͷ��壬
                 exti�ⲿ�жϵ�, ������ģ�����ֻҪ���漰��Щ�ģ���Ҫ���� #include"sys.h"
**************************************************************************************************
--�޸���ʷ��1) 2013/4/2������һ���궨��" #define USE_uCOS_II_EN  0u  "
               ֻҪ������uCOS-II,�궨��OS_VERSION�ͻᱻ���壬������OS_VERSION��ΪԤ��������
            2) ����void INTX_DISABLE(void)��void INTX_ENABLE(void)��������
               ������51��EA=1��EA=0
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
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//��2
}

//ʱ�����
void mco_out(void)
{
	RCC->APB2ENR |= RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA;

    GPIOA->CRH &= 0xFFFFFFF0;
    GPIOA->CRH |= 0x0000000B;//PA8
    GPIOA->ODR |= 0x0100;
    RCC->CFGR &=0xF0FFFFFF;
    RCC->CFGR |=0x05000000; //4:sysclk���   72MHz
                            //5:�ڲ�8MHz��RC 8MHz
                            //6:�ⲿ4~25MHz  8MHz
                            //7:PLLʱ��2��Ƶ 36MHz
}

/*************************************************************************************************
--�������ܣ�JTAGģʽ����      (�û�����)
--�����б�mode :  00, JTAG_SWD_ENABLE;  01,SWD_ENABLE;  10,JTAG_SWD_DISABLE;
--�� �� ֵ����
--ע    ����1) JTAG_Set(JTAG_SWD_ENABLE);
*************************************************************************************************/
void jtag_set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR |= RCC_APB2Periph_AFIO;     //��������ʱ��
	AFIO->MAPR &= 0XF8FFFFFF; //���MAPR��[26:24]
	AFIO->MAPR |= temp;       //����jtagģʽ
}

/*************************************************************************************************
--�������ܣ�wait for interrupt���ָ��  (�û�����)
--�����б���
--�� �� ֵ����
--ע    ����1) THUMBָ�֧�ֻ���������������·���ʵ��ִ�л��ָ��WFI
*************************************************************************************************/
__asm void WFI_SET(void)
{
	WFI;
}

/*************************************************************************************************
--�������ܣ��ر������ж�  (�û�����)
--�����б���
--�� �� ֵ����
--ע    ����1) �������
*************************************************************************************************/
__asm void INTX_DISABLE(void)
{
	CPSID I;
}

/*************************************************************************************************
--�������ܣ����������ж�  (�û�����)
--�����б���
--�� �� ֵ����
--ע    ����1) �������
*************************************************************************************************/
__asm void INTX_ENABLE(void)
{
	CPSIE I;
}

/*************************************************************************************************
--�������ܣ�����ջ����ַ (�û�����)
--�����б�addr:ջ����ַ
--�� �� ֵ����
--ע    ����1) �������
*************************************************************************************************/
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 //set Main Stack value
    BX r14
}

