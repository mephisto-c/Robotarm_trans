#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "bsp.h"

//主函数
int main(void)
{
	u32 ocnt=0;
	hw_init();

	for(;;)
	{
		if((SYS_CNT/1000)!=ocnt)//1s打印一次
		{
			
			ocnt = SYS_CNT/1000;
			xprintf("%d\n", SYS_CNT/1000);
			
			
		}
	}
}


