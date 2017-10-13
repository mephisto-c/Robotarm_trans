//-------------------------------------------------
//超声波
int main(void)
{
	u32 blink=0;


/*------------------------------------------------------*/
    Stm32_Clock_Init(9);//系统时钟设置
    delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化
	JTAG_Set(JTAG_SWD_DISABLE);//禁止JTAG
    RTC_Init();         //给ff提供时间
	LED_Init();         //PE5 PE6
    KEY_Init();         //PA0 PA2 PC0 PD12
	LCD_Init();         //PD0 PD1 PD4 PD5 PD7 PD8 PD9 PD10 PD11 PD13 PD14 PD15 PE1 PE7..PE15
    LCD_PWM_Init();     //	123456789012345678901234567

	super_init();
	LCD_ShowStr(10,50,16,0,"Super Wave Measure Distance");


	for(;;)
	{
		delay_ms(1);
		blink++;
		if(blink>100)
		{
			super_measure_dis();//100ms测一次
			blink=0;
			LED0=!LED0;
			LCD_ShowInfoNum(50,80,16,0,"dis(cm):",5,super_dis);
		}
	}
}

//--------------------------------------------------
//设置USB 连接/断线
//enable:0,断开
//       1,允许连接
void usb_port_set(u8 enable)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
	if(enable)
		_SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
	else
	{
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH &= 0XFFF00FFF;
		GPIOA->CRH |= 0X00033000;
		PAout(12)=0;
	}
}

int main(void)
{
	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA=0;
	u8 Divece_STA=0;



   	Stm32_Clock_Init(9);
	delay_init(72);
	uart_init(72,9600);
	LCD_Init();
	LED_Init();
	KEY_Init();



 	POINT_COLOR=RED;//设置字体为红色
	LCD_ShowStr(60,70,16,1,"USB Card Reader TEST");
// 	SPI_Flash_Init();
	if(SD_Initialize())
		LCD_ShowStr(60,130,16,1,"SD Card Error!");	//检测SD卡错误
	else //SD 卡正常
	{
		LCD_ShowStr(60,130,16,1,"SD Card Size:     MB");
 	    Mass_Memory_Size[0] = (long long)SD_GetSectorCount()*512;//得到SD卡容量（字节），当SD卡容量超过4G的时候,需要用到两个u32来表示
	    Mass_Block_Size[0]  = 512;//因为我们在Init里面设置了SD卡的操作字节为512个,所以这里一定是512个字节.
	    Mass_Block_Count[0] = Mass_Memory_Size[0]/Mass_Block_Size[0];
 		LCD_Show10Num(164,130,16, 0, 5, Mass_Memory_Size[0]>>20);	//显示SD卡容量
 	}
//	if(SPI_FLASH_TYPE!=W25Q64)
//		LCD_ShowStr(60,130,16,1,"W25Q64 Error!");	//检测SD卡错误
//	else //SPI FLASH 正常
//	{
// 	   	Mass_Memory_Size[1] = 1024*1024*6;//前6M字节
//	    Mass_Block_Size[1]  = 512;//因为我们在Init里面设置了SD卡的操作字节为512个,所以这里一定是512个字节.
//	    Mass_Block_Count[1] = Mass_Memory_Size[1]/Mass_Block_Size[1];
//		LCD_ShowStr(60,150,16,1,"SPI FLASH Size:6144KB");
//	}

	delay_ms(1800);
 	usb_port_set(0); 	//USB先断开
	delay_ms(300);
   	usb_port_set(1);	//USB再次连接
 	LCD_ShowStr(60,170,16,0,"USB Connecting...");//提示SD卡已经准备了


   	//USB配置
 	USB_Interrupts_Config();
 	Set_USBClock();
 	USB_Init();
	delay_ms(1800);



	for(;;)
	{
		delay_ms(1);
		if(USB_STA!=USB_STATUS_REG)//状态改变了
		{
			LCD_DrawFill(60,190,240,190+16, BACK_COLOR);//清除显示
			if(USB_STATUS_REG&0x01)//正在写
			{
				LCD_ShowStr(60,190,16,0,"USB Writing...");//提示USB正在写入数据
			}

			if(USB_STATUS_REG&0x02)//正在读
			{
				LCD_ShowStr(60,190,16,0,"USB Reading...");//提示USB正在读出数据
			}

			if(USB_STATUS_REG&0x04)
				LCD_ShowStr(60,210,16,0,"USB Write Err ");//提示写入错误
			else
				LCD_DrawFill(60,210,240,210+16, BACK_COLOR);//清除显示

			if(USB_STATUS_REG&0x08)
				LCD_ShowStr(60,230,16,0,"USB Read  Err ");//提示读出错误
			else
                LCD_DrawFill(60,230,240,230+16, BACK_COLOR);//清除显示

			USB_STA = USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState)
		{
			if(bDeviceState==CONFIGURED)
                LCD_ShowStr(60,170,16,0,"USB Connected    ");//提示USB连接已经建立
			else
                LCD_ShowStr(60,170,16,0,"USB DisConnected ");//提示USB被拔出了

			Divece_STA = bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct = 0;
			LED0 = !LED0;//提示系统在运行
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=CONFIGURED;
			}
			else//没有得到轮询
			{
				offline_cnt++;
				if(offline_cnt>10)
                    bDeviceState=UNCONNECTED;//2s内没收到在线标记,代表USB被拔出了
			}

			USB_STATUS_REG=0;
		}
	}
}


//随机函数显示图片
extern u8 gImage_fuck[];
extern u8 gImage_wash[];
extern u8 gImage_wash2[];

int main(void)
{
	u32 blink=0;
  	HEADCOLOR *imginfo=0;
	u16 x=0,y=0;
	u16 x0=0, y0=0;
 	imginfo = (HEADCOLOR*)gImage_fuck;	//得到文件信息

/*------------------------------------------------------*/
    Stm32_Clock_Init(9);//系统时钟设置
    delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化
    usmart_dev.init(72);
	JTAG_Set(0x02);//禁止JTAG
    RTC_Init();         //给ff提供时间
	LED_Init();         //PE5 PE6
    KEY_Init();         //PA0 PA2 PC0 PC3
	LCD_Init();         //PD0 PD1 PD4 PD5 PD7 PD8 PD9 PD10 PD11 PD13 PD14 PD15 PE1 PE7..PE15
    LCD_PWM_Init();     //


	srand(imginfo->h*imginfo->w);
	printf("fuck = %d", (long)gImage_fuck);
	printf("wash = %d", (long)gImage_wash);
	printf("wash2 = %d", (long)gImage_wash2);
	for(;;)
	{
		if(blink==0)
		{
			LCD_Clear(0);//黑屏
			if(imginfo->w >= LCD_SIZE_X || imginfo->h >= LCD_SIZE_Y)
			{
				POINT_COLOR=RED;
				LCD_ShowStr(10,70,16,1,"The Picture is too large");
				continue;
			}
			x0=x; y0=y;
			while((x+imginfo->w) > LCD_SIZE_X || x==x0)//超过屏幕尺寸了
			{
				x=rand();//获得随机的x值
			}
			while((y+imginfo->h)>LCD_SIZE_Y || y==y0)//超过屏幕尺寸了
			{
				y=rand();//获得随机的y值
			}
			image_display(x, y, (u8*)gImage_fuck);//在指定地址显示图片
		}
		blink++;
		if(blink>10)
		{
			blink=0;
			LED0 = !LED0;
		}
		delay_ms(60);
	}
}



//时钟输出
void mco(void)
{
	RCC->APB2ENR|=1<<0;     //开启AFIO时钟
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟

    GPIOA->CRH&=0xFFFFFFF0;
    GPIOA->CRH|=0x0000000B;//PA8
    GPIOA->ODR|=0x0100;
    RCC->CFGR &=0xF0FFFFFF;
    RCC->CFGR |=0x05000000; //4:sysclk输出   72MHz
                            //5:内部8MHz的RC 8MHz
                            //6:外部4~25MHz  8MHz
                            //7:PLL时钟2分频 36MHz
}

//-----------------------------------------------------
//手写识别
atk_ncr_point READ_BUF[200];
//主函数
int main(void)
{
 	u8 i=0;
	u8 tcnt=0;
	u8 res[10];//存储识别结果
	u8 key;
	u16 pcnt=0;
	u8 mode=4;	//默认是混合模式
	
	hw_init();
	alientek_ncr_init();
    POINT_COLOR=RED;
	Show_Str(60,70,16,0,"KEY0:MODE KEY2:Adjust",LCD_SIZE_X,16);
	Show_Str(60,90,16,0,"识别结果:", LCD_SIZE_X,16);
	LCD_DrawRect(19,114,220,315);
 	POINT_COLOR=BLUE;
	Show_Str(96,207,16,0,"手写区", LCD_SIZE_X, 16);
	tcnt=100;
	for(;;)
	{
		key=key_scan(0);
		if(key==KEY2_DOWN)
		{
			LCD_DrawFill(20,115,219,314,WHITE);//清除当前显示
			mode++;
			if(mode>4)
                mode=1;
			switch(mode)
			{
				case 1:
					Show_Str(80,207,16,0,"仅识别数字", LCD_SIZE_X, 16);
					break;
				case 2:
					Show_Str(64,207,16,0,"仅识别大写字母", LCD_SIZE_X,16);
					break;
				case 3:
					Show_Str(64,207,16,0,"仅识别小写字母", LCD_SIZE_X, 16);
					break;
				case 4:
					Show_Str(88,207,16,0,"全部识别", LCD_SIZE_X, 16);
					break;
			}
			tcnt=100;
		}
 		tp_dev.scan(0);//扫描
 		if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
		{
			delay_ms(1);//必要的延时,否则老认为有按键按下.
 			tcnt=0;//识别时的计数器清空
			if((tp_dev.x<220 && tp_dev.x>=20) && (tp_dev.y<315 && tp_dev.y>=115))
			{
				TP_Draw_Big_Point(tp_dev.x, tp_dev.y, BLUE);//画图
				if(pcnt<200)//总点数少于200
				{
					if(pcnt)
					{
						if((READ_BUF[pcnt-1].y != tp_dev.y) && (READ_BUF[pcnt-1].x != tp_dev.x))
						{//坐标不相等就存起来，也就是画了重复的点不计算在内
							READ_BUF[pcnt].x = tp_dev.x;//把点坐标存起来，之后用来识别
							READ_BUF[pcnt].y = tp_dev.y;
							pcnt++;
						}
					}
                    else
					{
						READ_BUF[pcnt].x = tp_dev.x;
						READ_BUF[pcnt].y = tp_dev.y;
						pcnt++;
					}
				}
			}
		}
        else //按键松开了
		{
			tcnt++;
			delay_ms(10);
			//延时识别
			i++;
			if(tcnt==40)//400ms没有碰屏幕就开始识别刚才手写的字符
			{
				if(pcnt)//有有效的输入
				{
					xprintf("总点数:%d\r\n",pcnt);
					alientek_ncr(READ_BUF, pcnt, 6, mode, (char*)res);
					xprintf("识别结果:%s\r\n",res);
					pcnt=0;
	  				POINT_COLOR=BLUE;//设置画笔蓝色
		 			LCD_ShowStr(60+72,90,16,0,(char*)res);
				}
				LCD_DrawFill(20,115,219,314,WHITE);
			}
		}
		if(i==30)
		{
			i=0;
			LED0=!LED0;
		}
	}
}

//--------------------------------------------------
//拼音输入法
//数字表
const u8* kbd_tbl[9]={"←","2","3","4","5","6","7","8","9",};
//字符表
const u8* kbs_tbl[9]={"DEL","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz",};

//加载键盘界面
//x,y:界面起始坐标
void py_load_ui(u16 x,u16 y)
{
	u16 i;
	POINT_COLOR=RED;
	LCD_DrawRect(x,y,x+180,y+120);
	LCD_DrawRect(x+60,y,x+120,y+120);
	LCD_DrawRect(x,y+40,x+180,y+80);
	POINT_COLOR=BLUE;
	for(i=0;i<9;i++)
	{
		Show_Str_Mid(x+(i%3)*60, y+4+40*(i/3), 16, 60, (u8*)kbd_tbl[i]);
		Show_Str_Mid(x+(i%3)*60, y+20+40*(i/3), 16, 60, (u8*)kbs_tbl[i]);
	}
}
//按键状态设置
//x,y:键盘坐标
//key:键值（0~8）
//sta:状态，0，松开；1，按下；
void py_key_staset(u16 x,u16 y,u8 keyx,u8 sta)
{
	u16 i=keyx/3,j=keyx%3;
	if(keyx>8)
        return;
	if(sta)
        LCD_DrawFill(x+j*60+1, y+i*40+1, x+j*60+59, y+i*40+39, GREEN);
	else
        LCD_DrawFill(x+j*60+1, y+i*40+1, x+j*60+59, y+i*40+39, WHITE);
	Show_Str_Mid(x+j*60, y+4+40*i, 16, 60, (u8*)kbd_tbl[keyx]);
	Show_Str_Mid(x+j*60, y+20+40*i, 16, 60, (u8*)kbs_tbl[keyx]);
}
//得到触摸屏的输入
//x,y:键盘坐标
//返回值：按键键值（1~9有效；0,无效）
u8 py_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,没有任何按键按下；1~9，1~9号按键按下
	u8 key=0;

	tp_dev.scan(0);
	if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
	{
		for(i=0;i<3;i++)
		{
			for(j=0;j<3;j++)
			{
			 	if(  tp_dev.x<(x+j*60+60) && tp_dev.x>(x+j*60) &&
                     tp_dev.y<(y+i*40+40) && tp_dev.y>(y+i*40)  )
				{
					key=i*3+j+1;
					break;
				}
			}
			if(key)
			{
				if(key_x==key)
                    key=0;
				else
				{
					py_key_staset(x,y,key_x-1,0);
					key_x=key;
					py_key_staset(x,y,key_x-1,1);
				}
				break;
			}
		}
	}
    else if(key_x)
	{
		py_key_staset(x,y,key_x-1,0);
		key_x=0;
	}
	return key;
}

//显示结果.
//index:0,表示没有一个匹配的结果.清空之前的显示
//   其他,索引号
void py_show_result(u8 index)
{
 	LCD_Show10Num(30+144,125,16,0,1,index); 		//显示当前的索引
	LCD_DrawFill(30+40,125,30+40+48,130+16,WHITE);	//清除之前的显示
 	LCD_DrawFill(30+40,145,30+200,145+48,WHITE);	//清除之前的显示
	if(index)
	{
 		Show_Str(30+40,125,16,0, t9.pymb[index-1]->py,200,16); 	//显示拼音
		Show_Str(30+40,145,16,0, t9.pymb[index-1]->pymb,240-70,48); //显示对应的汉字
		xprintf("\r\n拼音:%s\r\n", t9.pymb[index-1]->py);	//串口输出拼音
		xprintf("结果:%s\r\n",    t9.pymb[index-1]->pymb);	//串口输出结果
	}
}


int main(void)
{
 	u8 i=0;
	u8 result_num;
	u8 cur_index;
	u8 key;
	u8 inputstr[7];		//最大输入6个字符+结束符
	u8 inputlen;		//输入长度

/*------------------------------------------------------*/
    Stm32_Clock_Init(9);//系统时钟设置
    delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化
    usmart_dev.init(72);
	JTAG_Set(JTAG_SWD_DISABLE);//禁止JTAG
    RTC_Init();         //给ff提供时间
	LED_Init();         //PE5 PE6
    KEY_Init();         //PA0 PA2 PC0 PD12
	LCD_Init();         //PD0 PD1 PD4 PD5 PD7 PD8 PD9 PD10 PD11 PD13 PD14 PD15 PE1 PE7..PE15
	LCD_Clear(WHITE);
	BACK_COLOR=WHITE;

    LCD_PWM_Init();     //
    gui_init();         //自定义的gui要初始化

    mem_init(SRAMIN);   //内部内存池
    exf_init();         //为fatfs相关变量申请内存
    f_mount(0, fs[0]);  //挂载文件系统，申请工作空间, PA3

 	tp_dev.init();      //PA5 PA6 PA7 PB6 PB7
    font_init();        //PA5 PA6 PA7 PA4，字体初始化一定要放在触摸芯片后面
    if(KEY_Scan(1)==KEY2_DOWN)
        tp_dev.adjust();//强制校准屏幕
    TPAD_Init(72);      //PA1
	alientek_ncr_init();//初始化手写识别
/*-----------------------------------------------------*/

RESTART:
 	Show_Str(30,65,16,0,"  KEY2:校准  KEY0:清除",240,16);
 	Show_Str(30,85,16,0,"KEY_UP:上翻  KEY1:下翻",240,16);
	Show_Str(30,105,16,0,"输入:        匹配:  ",240,16);
 	Show_Str(30,125,16,0,"拼音:        当前:  ",240,16);
 	Show_Str(30,145,16,0,"结果:",240,16);
	py_load_ui(30,195);
	memset(inputstr,0,7);	//全部清零
	inputlen=0;				//输入长度为0
	result_num=0;			//总匹配数清零
	cur_index=0;
	while(1)
	{
		i++;
		delay_ms(10);
		key=py_get_keynum(30,195);
		if(key)
		{
			if(key==1)//删除
			{
				if(inputlen)
                    inputlen--;
				inputstr[inputlen]='\0';//添加结束符
			}
            else
			{
				inputstr[inputlen]=key+'0';//输入字符
				if(inputlen<7)
                    inputlen++;
			}
			if(inputstr[0]!=NULL)
			{
				key=t9.getpymb(inputstr);	//得到匹配的结果数
				if(key)//有部分匹配/完全匹配的结果
				{
					result_num=key;			//总匹配结果
					cur_index=1;			//当前为第一个索引
					if(key&0X80)		   	//是部分匹配
					{
						inputlen=key&0X7F;	//有效匹配位数
						inputstr[inputlen]='\0';//不匹配的位数去掉
						if(inputlen>1)
                            result_num=t9.getpymb(inputstr);//重新获取完全匹配字符数
					}
				}
                else 						//没有任何匹配
				{
					inputlen--;
					inputstr[inputlen]='\0';
				}
			}
            else
			{
				cur_index=0;
				result_num=0;
			}
			LCD_DrawFill(30+40,105,30+40+48,110+16,WHITE);	//清除之前的显示
			LCD_Show10Num(30+144,105,16,0,1,result_num); 	//显示匹配的结果数
			Show_Str(30+40,105,16,0,inputstr,200,16);	//显示有效的数字串
	 		py_show_result(cur_index);					//显示第cur_index的匹配结果
		}
		if(result_num)	//存在匹配的结果
		{
			key=KEY_Scan(0);
			switch(key)
			{
				case KEY2_DOWN://上翻
					if(cur_index<result_num)
                        cur_index++;
					else
                        cur_index=1;
					py_show_result(cur_index);	//显示第cur_index的匹配结果
					break;
 				case KEY3_DOWN://下翻
	   				if(cur_index>1)
                        cur_index--;
					else
                        cur_index=result_num;
					py_show_result(cur_index);	//显示第cur_index的匹配结果
					break;
				case KEY1_DOWN://清除输入
 					LCD_DrawFill(30+40,145,30+200,145+48,WHITE);	//清除之前的显示
					goto RESTART;
				case KEY4_DOWN://重新校准
					tp_dev.adjust();
					LCD_Clear(WHITE);
					goto RESTART;
			}
		}
		if(i==30)
		{
			i=0;
			LED0=!LED0;
		}
	}
}

//-----------------------------------------------------------
_btn_obj *m_btn[6];
//创建6个按钮
void mybtn_init(void)
{
    u8 i;
    for(i=0; i<6; i++)
        m_btn[i]=btn_creat(0+i*40+2, 296, 36, 20, i, BTN_TYPE_ANG);
    m_btn[0]->caption="确认"; m_btn[0]->font=12;
    m_btn[1]->caption="↑"; m_btn[1]->font=12;
    m_btn[2]->caption="↓"; m_btn[2]->font=12;
    m_btn[3]->caption="←"; m_btn[3]->font=12;
    m_btn[4]->caption="→"; m_btn[4]->font=12;
    m_btn[5]->caption="返回"; m_btn[5]->font=12;
    for(i=0; i<6; i++)
        btn_draw(m_btn[i]);
}
//        tp_dev.scan(0);
//        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);//得到按键键值
//
//        for(i=0; i<6; i++)
//        {
//            res=btn_check(m_btn[i], &in_obj);//按钮被按下的效果
//            if(res && ((m_btn[i]->sta&(1<<7))==0) && (m_btn[i]->sta&(1<<6)) )//物理上松开，逻辑上按下
//            {
//                switch(i)
//                {
//                    case 0:
//                        cnt++;
//                        break;
//                    case 1:
//                        cnt--;
//                        break;
//                    case 2:
//                        cnt=0;
//                        break;
//                    case 3:
//                        cnt=100;
//                        break;
//                    case 4:
//                        cnt=200;
//                        break;
//                    case 5:
//                        cnt=300;
//                        break;
//                }
//                LCD_Show10Num(50,150,16,0,4,cnt);        
//            }
//        }
//





//----------------------------------------------
//图片显示例程
//得到path路径下,目标文件的总个数
//path:路径
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息
	u8 *fn;

    res=f_opendir(&tdir,(const TCHAR*)path); //打开目录
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||tfileinfo.fname[0]==0)
                break;	//错误了/到末尾了,退出
     		fn = (u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);
			res = exf_typetell(fn);
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件
			{
				rval++;//有效文件数增加1
			}
		}
	}
	return rval;
}


int main(void)
{
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO picfileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 pause=0;			//暂停标记
	u8 t;
	u16 temp;
	u16 *picindextbl;	//图片索引表


/*----------------------各种初始化---------------------------*/
    Stm32_Clock_Init(9);
    delay_init(72);
    uart_init(72,9600);
    LED_Init();
    KEY_Init();
    LCD_Init();


    tp_dev.init();       //触摸屏初始化
    mem_init(SRAMIN);   //初始化内部内存池
    exf_init();         //为fatfs相关变量申请内存
    f_mount(0, fs[0]); //挂载文件系统，申请工作空间
    while(font_init())
    {
        update_font(16,20,16);
    }
/*-------------------------------------------------------------*/

	Show_Str(60,70,16,0,"图片显示程序",LCD_SIZE_X,16);
	Show_Str(60,90,16,0,"KEY0:NEXT KEY2:PREV", LCD_SIZE_X,16);
	Show_Str(60,130,16,0,"2012年9月19日", LCD_SIZE_X,16);
 	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{
		Show_Str(60,150,16,0,"PICTURE文件夹错误!", LCD_SIZE_X, 16);
		delay_ms(200);
		LCD_DrawFill(60,150,240,166,WHITE);//清除显示
		delay_ms(200);
	}
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0
 	{
		Show_Str(60,150,16,0,"没有图片文件!", LCD_SIZE_X, 16);
		delay_ms(200);
		LCD_DrawFill(60,150,240,166,WHITE);//清除显示
		delay_ms(200);
	}
  	picfileinfo.lfsize = _MAX_LFN*2+1;						//长文件名最大长度
	picfileinfo.lfname = mymalloc(SRAMIN,picfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname = mymalloc(SRAMIN,picfileinfo.lfsize);				//为带路径的文件名分配内存
 	picindextbl = mymalloc(SRAMIN,2*totpicnum);				//申请2*totpicnum个字节的内存,用于存放图片索引
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//内存分配出错
 	{
		Show_Str(60,150,16,0,"内存分配失败!", LCD_SIZE_X, 16);
		delay_ms(200);
		LCD_DrawFill(60,150,240,146,WHITE);//清除显示
		delay_ms(200);
	}
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp = picdir.index;								//记录当前index
	        res = f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo.fname[0]==0)
                break;	//错误了/到末尾了,退出
     		fn = (u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);
			res = exf_typetell(fn);
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件
			{
				picindextbl[curindex]=temp;//记录索引
				curindex++;
			}
		}
	}
	Show_Str(60,150,16,0,"开始显示...", LCD_SIZE_X, 16);
	delay_ms(1500);
	piclib_init();										//初始化画图
  	curindex=0;											//从0开始显示
   	res = f_opendir(&picdir, (const TCHAR*)"0:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{
		dir_sdi(&picdir, picindextbl[curindex]);			//改变当前目录索引
        res = f_readdir(&picdir, &picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo.fname[0]==0)
            break;	//错误了/到末尾了,退出
     	fn = (u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0, LCD_SIZE_X, LCD_SIZE_Y );//显示图片
		Show_Str(2,2,16,0,pname, LCD_SIZE_X,16);//显示图片名字
		t=0;
		while(1)
		{
			key=KEY_Scan(0);		//扫描按键
			if(t>250)
                key=KEY1_DOWN;	//模拟一次按下右键
			if(key==KEY2_DOWN)		//上一张
			{
				if(curindex)
                    curindex--;
				else
                    curindex=totpicnum-1;
				break;
			}
            else if(key==KEY1_DOWN)//下一张
			{
				curindex++;
				if(curindex>=totpicnum)
                    curindex=0;//到末尾的时候,自动从头开始
				break;
			}
            else if(key==KEY3_DOWN)
			{
				pause=!pause;
				LED1=!pause; 	//暂停的时候LED1亮.
			}
			if(pause==0)
                t++;
			delay_ms(10);
		}
		res=0;
	}
	myfree(SRAMIN,picfileinfo.lfname);	//释放内存
	myfree(SRAMIN,pname);				//释放内存
	myfree(SRAMIN,picindextbl);			//释放内存
}



//显示根目录下的文件
void scan_sd_file(void)
{
    u8 cnt=0;
    char *fn=0;
    FRESULT res;
	FILINFO *fileinfo;
	DIR dir;
	
	fileinfo = mymalloc(SRAMIN, sizeof(FILINFO));
#if _USE_LFN
    fileinfo->lfsize = _MAX_LFN * 2 + 1;
    fileinfo->lfname = mymalloc(SRAMIN, fileinfo->lfsize);
#endif
    POINT_COLOR=RED;
    BACK_COLOR=WHITE;

    LCD_Clear(WHITE);
    Show_Str(70,0,16,1,"fatfs文件浏览",200,16);

    f_opendir(&dir, "0:");                      //打开根目录
    LCD_ShowStr(10,20,16,1, "name             type  size");
    //                       1234567890123456789012345678
    for(cnt=0; ; cnt++)
    {
        res = f_readdir(&dir, fileinfo);
        if(res != FR_OK || fileinfo->fname[0]==0)//目录项为空或错误
            break;
        if(fileinfo->fname[0]=='.')              //点目录(本级目录) 点点目录(上级目录)
            continue;
#if _USE_LFN
        fn = *fileinfo->lfname ? fileinfo->lfname : fileinfo->fname;//判断是否为长文件名
#else
        fn = fileinfo->fname;
#endif
        LCD_Show10Num(10+22*9, 40+cnt*16, 16,0,3, fileinfo->fsize);//显示文件大小
        Show_Str(10,40+cnt*16, 12,0, (u8*)fn, 200,12);//显示目录项名称
        if(fileinfo->fattrib == AM_DIR)
            LCD_ShowStr(10+8*17, 40+cnt*16, 16,0,"dir");//文件夹
        else
            LCD_ShowStr(10+8*17, 40+cnt*16, 16,0,"file");//文件
    }
#if _USE_LFN
    myfree(SRAMIN, fileinfo->lfname);            //释放长文件名内存
#endif
	myfree(SRAMIN, fileinfo);
}






//--------------------------------------------
//触控USB例程
void Load_Draw_Dialog(void)
{
	LCD_Clear(WHITE);//清屏
 	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowStr(lcd_dev.width-24,0,16,0,"RST");//显示清屏区域
  	POINT_COLOR=RED;//设置画笔蓝色
}
//计算x1,x2的绝对值
u32 usb_abs(u32 x1,u32 x2)
{
	if(x1>x2)return x1-x2;
	else return x2-x1;
}
//设置USB 连接/断线
//enable:0,断开
//       1,允许连接
void usb_port_set(u8 enable)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟
	if(enable)
        _SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
	else
	{
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;
	}
}

int main(void)
{
	u8 key=0;
	u8 i=0;
 	s8 x0=0;		 //发送到电脑端的坐标值
	s8 y0=0;
	u8 keysta=0;	//[0]:0,左键松开;1,左键按下;
					//[1]:0,右键松开;1,右键按下
					//[2]:0,中键松开;1,中键按下
	u8 tpsta=0;		//0,触摸屏第一次按下;1,触摸屏滑动
	short xlast=0; //最后一次按下的坐标值
	short ylast=0;




    Stm32_Clock_Init(9);
    delay_init(72);
    uart_init(72,9600);
    JTAG_Set(0x02);

    LED_Init();
    KEY_Init();
    LCD_Init();
	LCD_PWM_Init();

    tp_dev.init();
	key = KEY_Scan(0);
	if(key==KEY1_DOWN)
		tp_dev.adjust();

	POINT_COLOR=RED;
	LCD_ShowStr(60,130,16,1,"KEY_UP:SCROLL +");
	LCD_ShowStr(60,150,16,1,"KEY_DOWN:SCROLL -");
	LCD_ShowStr(60,170,16,1,"KEY_RIGHT:RIGHT BTN");
	LCD_ShowStr(60,190,16,1,"KEY_LEFT:LEFT BTN");
  	delay_ms(1800);
 	usb_port_set(0); 	//USB先断开
	delay_ms(300);
   	usb_port_set(1);	//USB再次连接
	//USB配置
 	USB_Interrupts_Config();
 	Set_USBClock();
 	USB_Init();
	Load_Draw_Dialog();


 	while(1)
	{
		key=KEY_Scan(1);//支持连按

		if(key)
		{
			if(key==KEY1_DOWN)
                Joystick_Send(0,0,0,1);		//发送滚轮数据到电脑
			else if(key==KEY2_DOWN)
                Joystick_Send(0,0,0,(u8)-1);//发送滚轮数据到电脑
			else
			{
				if(key==KEY3_DOWN)
                    keysta|=0X01;			//发送鼠标左键
				if(key==KEY4_DOWN)
                    keysta|=0X02;			//发送鼠标右键
				Joystick_Send(keysta,0,0,0);//发送给电脑
			}
		}
        else if(keysta)//之前有按下
		{
			keysta=0;
			Joystick_Send(0,0,0,0); //发送松开命令给电脑
		}



		tp_dev.scan(0);
		if(tp_dev.sta&0x80)//触摸屏被按下
		{
			//最少移动5个单位,才算滑动
  			if( ((usb_abs(tp_dev.x, xlast)>4) || (usb_abs(tp_dev.y,ylast)>4)) &&
                tpsta==0 )//等待滑动
			{
				xlast=tp_dev.x;		//记录刚按下的坐标
				ylast=tp_dev.y;
	 			tpsta=1;
			}
			if(tp_dev.x>216&&tp_dev.y<16)
                Load_Draw_Dialog();//清除
			else
                TP_Draw_Big_Point(tp_dev.x,tp_dev.y,RED);		//画图
			if(bDeviceState==CONFIGURED)
			{
				if(tpsta)//滑动
				{
					x0    = (xlast-tp_dev.x)*3;	//上次坐标值与得到的坐标值之差,扩大2倍
					y0    = (ylast-tp_dev.y)*3;
					xlast = tp_dev.x;			//记录刚按下的坐标
					ylast = tp_dev.y;
					Joystick_Send(keysta,-x0,-y0,0); //发送数据到电脑
					delay_ms(5);
				}
			}
		}
        else
		{
			tpsta=0;	//清除
 			delay_ms(1);
		}


		if(bDeviceState==CONFIGURED)
            LED1=0;//当USB配置成功了，LED1亮，否则，灭
		else 
            LED1=1;
		i++;

		if(i==200)
		{
			i=0;
			LED0=!LED0;
		}
	}
}
