/*************************************************************************************************
                                  STM32 SysTickģ�����
--�ļ���    : delay.c
--�汾      : V1.0
--����      : ������
--��������  : 2013/2/14
--�޸�����  : 2013/2/14
--ע��      : 1) �����SysTick������ʱ�����������ʱ��ȷ��
              2) SysTick�����¼���, SysTick->VAL�Ƶ�0ʱ���ֻ��SysTick->LOAD��ȡ����װֵ��
                 �ټ������¼���.
              3) 2013/4/2��ʹ��uCOS-IIϵͳʱ�����Ե���delay_ms��delay_us����������ʱ��
                           Ҳ���Ե���OSTimeDly()��OSTimeDlyHSMM()����������ʱ��
--Copyright(C) ZRP 2012-2022  All rights reserved
**************************************************************************************************
--�޸���ʷ��2013/4/2�����˶�uCOS-II��֧�֣�ʹ��OSʱ��SysTick����ʱ�ӽ��ļ���
              ֻҪ������uCOS-II,�궨��OS_VERSION�ͻᱻ���壬������OS_VERSION��ΪԤ��������
*************************************************************************************************/
#include "delay.h"

#if  USE_uCOS_II_EN > 0u //���ʹ�õ�uCOS-II���Ͱ������ͷ�ļ�
#include "includes.h"
#endif

static u8  fac_us=0;   //1us��ʱ��Ҫ��SysTick->LOAD��װ����ֵ
static u16 fac_ms=0;   //1ms��ʱ��Ҫ��SysTick->LOAD��װ����ֵ

#ifdef OS_VERSION
void SysTick_Handler(void)
{
    OSIntEnter();      //�����ж�
    OSTimeTick();      //����uCOS-II�ĵδ���
    OSIntExit();       //�˳��ж�
}
#endif
/*************************************************************************************************
--�������ܣ���ʼ��systick     (�û�����)
--�����б�SYSCLK : ϵͳ��ʱ��
--�� �� ֵ����
--ע    ����1) SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8��
            2) delay_init(72);
            3) OS_TICKS_PER_SEC���� 1s ���ж��ٴεδ�
*************************************************************************************************/
void delay_init(u8 sysclk)
{
#ifdef OS_VERSION//���������uCOS-II��SysTick����Ϊϵͳ��ʱ�ӽ���
    u32 reload=0;
#endif
	SysTick->CTRL &= ~(1<<2);//bit2���,ѡ���ⲿʱ��
	fac_us = sysclk/8;       //��Ϊ��HCLK/8������72MHz��9��SysTickΪ1us

#ifdef OS_VERSION
    reload=sysclk/8;         //reload��ʱ����1us����װֵ������������*1000000��������1s
    reload=reload*1000000/OS_TICKS_PER_SEC;//uCOS-II��1s(1000000us), һ��ʱ�ӽ����Ƕ೤ʱ��
                                           //����Ҫ����һ��ʱ�ӽ���Ӧ����װ������ֵ
    fac_ms=1000/OS_TICKS_PER_SEC;//fac_ms = (1/OS_TICKS_PER_SEC) = OS��С��ʱ��
	SysTick->CTRL|=1<<1;   	//����SysTick�ж�
	SysTick->LOAD=reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��	
	SysTick->CTRL|=1<<0;   	//����SYSTICK
#else
	fac_ms = (u16)fac_us*1000;//ms��װֵ�ı���,SysTick������ͨ��ʱ��ʱ��,��uCOS-II��ʱ��
#endif
}

#ifdef OS_VERSION//������uCOS-II��ҲΪ���ṩ�����ǳ���ȷ����ʱ����
/*************************************************************************************************
--�������ܣ���uCOS-IIϵͳ�У���ʱnms   (�û�����)
--�����б�nms      Ҫ��ʱ��ms��
--�� �� ֵ����
--ע    ����1) SysTick->LOADΪ24λ, 8��tickΪ1us, ���������ʱΪ:nms<=(0xffffff/8)/1000
            2) nms/fac_ms,       ����OS��Сʱ���Ĳ�����OSTimeDly();
              (nms%fac_ms)*1000, С��OS��Сʱ���Ĳ�����delay_us();
*************************************************************************************************/
void delay_ms(u16 nms)
{
    if(OSRunning==TRUE)
    {
        if(nms>=fac_ms)//�����ʱ����OS����Сʱ��
        {
            OSTimeDly(nms/fac_ms);//nms/fac_ms,����OS��Сʱ���Ĳ�����OSTimeDly();
        }
    }
    delay_us((u32)(nms%fac_ms)*1000);//(nms%fac_ms)*1000, С��OS��Сʱ���Ĳ�����delay_us();
}

/*************************************************************************************************
--�������ܣ���uCOS-IIϵͳ�У���ʱnus   (�û�����)
--�����б�nus      Ҫ��ʱ��us��
--�� �� ֵ����
--ע    ����1) SysTick->LOADΪ24λ, 8��tickΪ1us, ���������ʱΪ:nms<=(0xffffff/8)/1000
            2) ��72M������, nms<=2097ms, SYSCLK��λΪHz,nms��λΪms
*************************************************************************************************/
void delay_us(u32 nus)
{
    u32 ticks=0, tnow=0, told=0, tcnt=0;
    u32 reload=SysTick->LOAD;
    ticks=nus*fac_us;       //�����������ʱ���ٸ�tick
    OSSchedLock();          //OS�������������
    told=SysTick->VAL;      //�����ֵ
    for(;;)
    {
        tnow=SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow<told)
                tcnt=tcnt+(told-tnow);//SysTick�����¼�����, ������told-tnow
            else tcnt+=reload+(told-tnow);
            told=tnow;          //toldһֱβ����tnow
            if(tcnt>ticks)break;//�����ﵽticks��
        }
    }
    OSSchedUnlock();        //�������������
}
#else
//��ʹ��uCOS-II
/*************************************************************************************************
--�������ܣ���ʱnms   (�û�����)
--�����б�nms      Ҫ��ʱ��ms��
--�� �� ֵ����
--ע    ����1) SysTick->LOADΪ24λ, 8��tickΪ1us, ���������ʱΪ:nms<=(0xffffff/8)/1000
            2) ��72M������, nms<=2097ms, SYSCLK��λΪHz,nms��λΪms
*************************************************************************************************/
void delay_ms(u16 nms)
{
	u32 temp;

	SysTick->LOAD = (u32)nms*fac_ms;  //ʱ�����
	SysTick->VAL = 0x00;              //��ռ�����
	SysTick->CTRL|= 1<<0;             //��������
	do{
		temp = SysTick->CTRL;
	}
	while( temp&0x01 && !( temp&0x10000 ) );//�ȴ�ʱ�䵽��
	SysTick->CTRL = 0x00;             //�رռ�����
	SysTick->VAL  = 0X00;             //��ռ�����
}

/*************************************************************************************************
--�������ܣ���ʱnus     (�û�����)
--�����б�nus     Ҫ��ʱ��us��
--�� �� ֵ����
--ע    ����1) 72MHz�£�8tick=1us, 0xFFFFFFtick=2 097 151.875us
            2) SysTick->LOADΪ24λ, 8��tickΪ1us
*************************************************************************************************/
void delay_us(u32 nus)
{
	u32 temp;

	SysTick->LOAD = nus*fac_us; //ʱ�����
	SysTick->VAL = 0x00;        //��ռ�����
	SysTick->CTRL = 0x01 ;      //��ʼ����
	do{
		temp = SysTick->CTRL;
	}
	while( temp&0x01 && !( temp&0x10000 ) );//�ȴ�ʱ�䵽��
	SysTick->CTRL = 0x00;       //�رռ�����
	SysTick->VAL  = 0X00;       //��ռ�����
}
#endif





