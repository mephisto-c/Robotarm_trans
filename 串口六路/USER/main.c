#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "bsp.h"

//������
int main(void)
{
	u32 ocnt=0;
	hw_init();

	for(;;)
	{
		if((SYS_CNT/1000)!=ocnt)//1s��ӡһ��
		{
			
			ocnt = SYS_CNT/1000;
			xprintf("%d\n", SYS_CNT/1000);
			
			
		}
	}
}


