#include "bsp.h"




//硬件初始化
void hw_init(void)
{
__SETPRIMASK();//关闭全局中断
{
	
	
      RCC_Configuration();
      delay_init(72);
      uart_init(9600);
      usmart_dev.init(72);
	steer_init();
	GPIOB_Init();
	TIM5_Int_Init(99,719);

}
__RESETPRIMASK();//开全局中断
}


