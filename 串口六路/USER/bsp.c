#include "bsp.h"




//Ӳ����ʼ��
void hw_init(void)
{
__SETPRIMASK();//�ر�ȫ���ж�
{
	
	
      RCC_Configuration();
      delay_init(72);
      uart_init(9600);
      usmart_dev.init(72);
	steer_init();
	GPIOB_Init();
	TIM5_Int_Init(99,719);

}
__RESETPRIMASK();//��ȫ���ж�
}


