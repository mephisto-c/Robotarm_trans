//-------------------------------------------------
//������
int main(void)
{
	u32 blink=0;


/*------------------------------------------------------*/
    Stm32_Clock_Init(9);//ϵͳʱ������
    delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE);//��ֹJTAG
    RTC_Init();         //��ff�ṩʱ��
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
			super_measure_dis();//100ms��һ��
			blink=0;
			LED0=!LED0;
			LCD_ShowInfoNum(50,80,16,0,"dis(cm):",5,super_dis);
		}
	}
}

//--------------------------------------------------
//����USB ����/����
//enable:0,�Ͽ�
//       1,��������
void usb_port_set(u8 enable)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
	if(enable)
		_SetCNTR(_GetCNTR()&(~(1<<1)));//�˳��ϵ�ģʽ
	else
	{
		_SetCNTR(_GetCNTR()|(1<<1));  // �ϵ�ģʽ
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



 	POINT_COLOR=RED;//��������Ϊ��ɫ
	LCD_ShowStr(60,70,16,1,"USB Card Reader TEST");
// 	SPI_Flash_Init();
	if(SD_Initialize())
		LCD_ShowStr(60,130,16,1,"SD Card Error!");	//���SD������
	else //SD ������
	{
		LCD_ShowStr(60,130,16,1,"SD Card Size:     MB");
 	    Mass_Memory_Size[0] = (long long)SD_GetSectorCount()*512;//�õ�SD���������ֽڣ�����SD����������4G��ʱ��,��Ҫ�õ�����u32����ʾ
	    Mass_Block_Size[0]  = 512;//��Ϊ������Init����������SD���Ĳ����ֽ�Ϊ512��,��������һ����512���ֽ�.
	    Mass_Block_Count[0] = Mass_Memory_Size[0]/Mass_Block_Size[0];
 		LCD_Show10Num(164,130,16, 0, 5, Mass_Memory_Size[0]>>20);	//��ʾSD������
 	}
//	if(SPI_FLASH_TYPE!=W25Q64)
//		LCD_ShowStr(60,130,16,1,"W25Q64 Error!");	//���SD������
//	else //SPI FLASH ����
//	{
// 	   	Mass_Memory_Size[1] = 1024*1024*6;//ǰ6M�ֽ�
//	    Mass_Block_Size[1]  = 512;//��Ϊ������Init����������SD���Ĳ����ֽ�Ϊ512��,��������һ����512���ֽ�.
//	    Mass_Block_Count[1] = Mass_Memory_Size[1]/Mass_Block_Size[1];
//		LCD_ShowStr(60,150,16,1,"SPI FLASH Size:6144KB");
//	}

	delay_ms(1800);
 	usb_port_set(0); 	//USB�ȶϿ�
	delay_ms(300);
   	usb_port_set(1);	//USB�ٴ�����
 	LCD_ShowStr(60,170,16,0,"USB Connecting...");//��ʾSD���Ѿ�׼����


   	//USB����
 	USB_Interrupts_Config();
 	Set_USBClock();
 	USB_Init();
	delay_ms(1800);



	for(;;)
	{
		delay_ms(1);
		if(USB_STA!=USB_STATUS_REG)//״̬�ı���
		{
			LCD_DrawFill(60,190,240,190+16, BACK_COLOR);//�����ʾ
			if(USB_STATUS_REG&0x01)//����д
			{
				LCD_ShowStr(60,190,16,0,"USB Writing...");//��ʾUSB����д������
			}

			if(USB_STATUS_REG&0x02)//���ڶ�
			{
				LCD_ShowStr(60,190,16,0,"USB Reading...");//��ʾUSB���ڶ�������
			}

			if(USB_STATUS_REG&0x04)
				LCD_ShowStr(60,210,16,0,"USB Write Err ");//��ʾд�����
			else
				LCD_DrawFill(60,210,240,210+16, BACK_COLOR);//�����ʾ

			if(USB_STATUS_REG&0x08)
				LCD_ShowStr(60,230,16,0,"USB Read  Err ");//��ʾ��������
			else
                LCD_DrawFill(60,230,240,230+16, BACK_COLOR);//�����ʾ

			USB_STA = USB_STATUS_REG;//��¼����״̬
		}
		if(Divece_STA!=bDeviceState)
		{
			if(bDeviceState==CONFIGURED)
                LCD_ShowStr(60,170,16,0,"USB Connected    ");//��ʾUSB�����Ѿ�����
			else
                LCD_ShowStr(60,170,16,0,"USB DisConnected ");//��ʾUSB���γ���

			Divece_STA = bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct = 0;
			LED0 = !LED0;//��ʾϵͳ������
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB������,�����offline������
				bDeviceState=CONFIGURED;
			}
			else//û�еõ���ѯ
			{
				offline_cnt++;
				if(offline_cnt>10)
                    bDeviceState=UNCONNECTED;//2s��û�յ����߱��,����USB���γ���
			}

			USB_STATUS_REG=0;
		}
	}
}


//���������ʾͼƬ
extern u8 gImage_fuck[];
extern u8 gImage_wash[];
extern u8 gImage_wash2[];

int main(void)
{
	u32 blink=0;
  	HEADCOLOR *imginfo=0;
	u16 x=0,y=0;
	u16 x0=0, y0=0;
 	imginfo = (HEADCOLOR*)gImage_fuck;	//�õ��ļ���Ϣ

/*------------------------------------------------------*/
    Stm32_Clock_Init(9);//ϵͳʱ������
    delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��
    usmart_dev.init(72);
	JTAG_Set(0x02);//��ֹJTAG
    RTC_Init();         //��ff�ṩʱ��
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
			LCD_Clear(0);//����
			if(imginfo->w >= LCD_SIZE_X || imginfo->h >= LCD_SIZE_Y)
			{
				POINT_COLOR=RED;
				LCD_ShowStr(10,70,16,1,"The Picture is too large");
				continue;
			}
			x0=x; y0=y;
			while((x+imginfo->w) > LCD_SIZE_X || x==x0)//������Ļ�ߴ���
			{
				x=rand();//��������xֵ
			}
			while((y+imginfo->h)>LCD_SIZE_Y || y==y0)//������Ļ�ߴ���
			{
				y=rand();//��������yֵ
			}
			image_display(x, y, (u8*)gImage_fuck);//��ָ����ַ��ʾͼƬ
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



//ʱ�����
void mco(void)
{
	RCC->APB2ENR|=1<<0;     //����AFIOʱ��
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��

    GPIOA->CRH&=0xFFFFFFF0;
    GPIOA->CRH|=0x0000000B;//PA8
    GPIOA->ODR|=0x0100;
    RCC->CFGR &=0xF0FFFFFF;
    RCC->CFGR |=0x05000000; //4:sysclk���   72MHz
                            //5:�ڲ�8MHz��RC 8MHz
                            //6:�ⲿ4~25MHz  8MHz
                            //7:PLLʱ��2��Ƶ 36MHz
}

//-----------------------------------------------------
//��дʶ��
atk_ncr_point READ_BUF[200];
//������
int main(void)
{
 	u8 i=0;
	u8 tcnt=0;
	u8 res[10];//�洢ʶ����
	u8 key;
	u16 pcnt=0;
	u8 mode=4;	//Ĭ���ǻ��ģʽ
	
	hw_init();
	alientek_ncr_init();
    POINT_COLOR=RED;
	Show_Str(60,70,16,0,"KEY0:MODE KEY2:Adjust",LCD_SIZE_X,16);
	Show_Str(60,90,16,0,"ʶ����:", LCD_SIZE_X,16);
	LCD_DrawRect(19,114,220,315);
 	POINT_COLOR=BLUE;
	Show_Str(96,207,16,0,"��д��", LCD_SIZE_X, 16);
	tcnt=100;
	for(;;)
	{
		key=key_scan(0);
		if(key==KEY2_DOWN)
		{
			LCD_DrawFill(20,115,219,314,WHITE);//�����ǰ��ʾ
			mode++;
			if(mode>4)
                mode=1;
			switch(mode)
			{
				case 1:
					Show_Str(80,207,16,0,"��ʶ������", LCD_SIZE_X, 16);
					break;
				case 2:
					Show_Str(64,207,16,0,"��ʶ���д��ĸ", LCD_SIZE_X,16);
					break;
				case 3:
					Show_Str(64,207,16,0,"��ʶ��Сд��ĸ", LCD_SIZE_X, 16);
					break;
				case 4:
					Show_Str(88,207,16,0,"ȫ��ʶ��", LCD_SIZE_X, 16);
					break;
			}
			tcnt=100;
		}
 		tp_dev.scan(0);//ɨ��
 		if(tp_dev.sta&TP_PRES_DOWN)//�а���������
		{
			delay_ms(1);//��Ҫ����ʱ,��������Ϊ�а�������.
 			tcnt=0;//ʶ��ʱ�ļ��������
			if((tp_dev.x<220 && tp_dev.x>=20) && (tp_dev.y<315 && tp_dev.y>=115))
			{
				TP_Draw_Big_Point(tp_dev.x, tp_dev.y, BLUE);//��ͼ
				if(pcnt<200)//�ܵ�������200
				{
					if(pcnt)
					{
						if((READ_BUF[pcnt-1].y != tp_dev.y) && (READ_BUF[pcnt-1].x != tp_dev.x))
						{//���겻��Ⱦʹ�������Ҳ���ǻ����ظ��ĵ㲻��������
							READ_BUF[pcnt].x = tp_dev.x;//�ѵ������������֮������ʶ��
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
        else //�����ɿ���
		{
			tcnt++;
			delay_ms(10);
			//��ʱʶ��
			i++;
			if(tcnt==40)//400msû������Ļ�Ϳ�ʼʶ��ղ���д���ַ�
			{
				if(pcnt)//����Ч������
				{
					xprintf("�ܵ���:%d\r\n",pcnt);
					alientek_ncr(READ_BUF, pcnt, 6, mode, (char*)res);
					xprintf("ʶ����:%s\r\n",res);
					pcnt=0;
	  				POINT_COLOR=BLUE;//���û�����ɫ
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
//ƴ�����뷨
//���ֱ�
const u8* kbd_tbl[9]={"��","2","3","4","5","6","7","8","9",};
//�ַ���
const u8* kbs_tbl[9]={"DEL","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz",};

//���ؼ��̽���
//x,y:������ʼ����
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
//����״̬����
//x,y:��������
//key:��ֵ��0~8��
//sta:״̬��0���ɿ���1�����£�
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
//�õ�������������
//x,y:��������
//����ֵ��������ֵ��1~9��Ч��0,��Ч��
u8 py_get_keynum(u16 x,u16 y)
{
	u16 i,j;
	static u8 key_x=0;//0,û���κΰ������£�1~9��1~9�Ű�������
	u8 key=0;

	tp_dev.scan(0);
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
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

//��ʾ���.
//index:0,��ʾû��һ��ƥ��Ľ��.���֮ǰ����ʾ
//   ����,������
void py_show_result(u8 index)
{
 	LCD_Show10Num(30+144,125,16,0,1,index); 		//��ʾ��ǰ������
	LCD_DrawFill(30+40,125,30+40+48,130+16,WHITE);	//���֮ǰ����ʾ
 	LCD_DrawFill(30+40,145,30+200,145+48,WHITE);	//���֮ǰ����ʾ
	if(index)
	{
 		Show_Str(30+40,125,16,0, t9.pymb[index-1]->py,200,16); 	//��ʾƴ��
		Show_Str(30+40,145,16,0, t9.pymb[index-1]->pymb,240-70,48); //��ʾ��Ӧ�ĺ���
		xprintf("\r\nƴ��:%s\r\n", t9.pymb[index-1]->py);	//�������ƴ��
		xprintf("���:%s\r\n",    t9.pymb[index-1]->pymb);	//����������
	}
}


int main(void)
{
 	u8 i=0;
	u8 result_num;
	u8 cur_index;
	u8 key;
	u8 inputstr[7];		//�������6���ַ�+������
	u8 inputlen;		//���볤��

/*------------------------------------------------------*/
    Stm32_Clock_Init(9);//ϵͳʱ������
    delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��
    usmart_dev.init(72);
	JTAG_Set(JTAG_SWD_DISABLE);//��ֹJTAG
    RTC_Init();         //��ff�ṩʱ��
	LED_Init();         //PE5 PE6
    KEY_Init();         //PA0 PA2 PC0 PD12
	LCD_Init();         //PD0 PD1 PD4 PD5 PD7 PD8 PD9 PD10 PD11 PD13 PD14 PD15 PE1 PE7..PE15
	LCD_Clear(WHITE);
	BACK_COLOR=WHITE;

    LCD_PWM_Init();     //
    gui_init();         //�Զ����guiҪ��ʼ��

    mem_init(SRAMIN);   //�ڲ��ڴ��
    exf_init();         //Ϊfatfs��ر��������ڴ�
    f_mount(0, fs[0]);  //�����ļ�ϵͳ�����빤���ռ�, PA3

 	tp_dev.init();      //PA5 PA6 PA7 PB6 PB7
    font_init();        //PA5 PA6 PA7 PA4�������ʼ��һ��Ҫ���ڴ���оƬ����
    if(KEY_Scan(1)==KEY2_DOWN)
        tp_dev.adjust();//ǿ��У׼��Ļ
    TPAD_Init(72);      //PA1
	alientek_ncr_init();//��ʼ����дʶ��
/*-----------------------------------------------------*/

RESTART:
 	Show_Str(30,65,16,0,"  KEY2:У׼  KEY0:���",240,16);
 	Show_Str(30,85,16,0,"KEY_UP:�Ϸ�  KEY1:�·�",240,16);
	Show_Str(30,105,16,0,"����:        ƥ��:  ",240,16);
 	Show_Str(30,125,16,0,"ƴ��:        ��ǰ:  ",240,16);
 	Show_Str(30,145,16,0,"���:",240,16);
	py_load_ui(30,195);
	memset(inputstr,0,7);	//ȫ������
	inputlen=0;				//���볤��Ϊ0
	result_num=0;			//��ƥ��������
	cur_index=0;
	while(1)
	{
		i++;
		delay_ms(10);
		key=py_get_keynum(30,195);
		if(key)
		{
			if(key==1)//ɾ��
			{
				if(inputlen)
                    inputlen--;
				inputstr[inputlen]='\0';//��ӽ�����
			}
            else
			{
				inputstr[inputlen]=key+'0';//�����ַ�
				if(inputlen<7)
                    inputlen++;
			}
			if(inputstr[0]!=NULL)
			{
				key=t9.getpymb(inputstr);	//�õ�ƥ��Ľ����
				if(key)//�в���ƥ��/��ȫƥ��Ľ��
				{
					result_num=key;			//��ƥ����
					cur_index=1;			//��ǰΪ��һ������
					if(key&0X80)		   	//�ǲ���ƥ��
					{
						inputlen=key&0X7F;	//��Чƥ��λ��
						inputstr[inputlen]='\0';//��ƥ���λ��ȥ��
						if(inputlen>1)
                            result_num=t9.getpymb(inputstr);//���»�ȡ��ȫƥ���ַ���
					}
				}
                else 						//û���κ�ƥ��
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
			LCD_DrawFill(30+40,105,30+40+48,110+16,WHITE);	//���֮ǰ����ʾ
			LCD_Show10Num(30+144,105,16,0,1,result_num); 	//��ʾƥ��Ľ����
			Show_Str(30+40,105,16,0,inputstr,200,16);	//��ʾ��Ч�����ִ�
	 		py_show_result(cur_index);					//��ʾ��cur_index��ƥ����
		}
		if(result_num)	//����ƥ��Ľ��
		{
			key=KEY_Scan(0);
			switch(key)
			{
				case KEY2_DOWN://�Ϸ�
					if(cur_index<result_num)
                        cur_index++;
					else
                        cur_index=1;
					py_show_result(cur_index);	//��ʾ��cur_index��ƥ����
					break;
 				case KEY3_DOWN://�·�
	   				if(cur_index>1)
                        cur_index--;
					else
                        cur_index=result_num;
					py_show_result(cur_index);	//��ʾ��cur_index��ƥ����
					break;
				case KEY1_DOWN://�������
 					LCD_DrawFill(30+40,145,30+200,145+48,WHITE);	//���֮ǰ����ʾ
					goto RESTART;
				case KEY4_DOWN://����У׼
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
//����6����ť
void mybtn_init(void)
{
    u8 i;
    for(i=0; i<6; i++)
        m_btn[i]=btn_creat(0+i*40+2, 296, 36, 20, i, BTN_TYPE_ANG);
    m_btn[0]->caption="ȷ��"; m_btn[0]->font=12;
    m_btn[1]->caption="��"; m_btn[1]->font=12;
    m_btn[2]->caption="��"; m_btn[2]->font=12;
    m_btn[3]->caption="��"; m_btn[3]->font=12;
    m_btn[4]->caption="��"; m_btn[4]->font=12;
    m_btn[5]->caption="����"; m_btn[5]->font=12;
    for(i=0; i<6; i++)
        btn_draw(m_btn[i]);
}
//        tp_dev.scan(0);
//        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);//�õ�������ֵ
//
//        for(i=0; i<6; i++)
//        {
//            res=btn_check(m_btn[i], &in_obj);//��ť�����µ�Ч��
//            if(res && ((m_btn[i]->sta&(1<<7))==0) && (m_btn[i]->sta&(1<<6)) )//�������ɿ����߼��ϰ���
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
//ͼƬ��ʾ����
//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ
	u8 *fn;

    res=f_opendir(&tdir,(const TCHAR*)path); //��Ŀ¼
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||tfileinfo.fname[0]==0)
                break;	//������/��ĩβ��,�˳�
     		fn = (u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);
			res = exf_typetell(fn);
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�
			{
				rval++;//��Ч�ļ�������1
			}
		}
	}
	return rval;
}


int main(void)
{
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO picfileinfo;//�ļ���Ϣ
	u8 *fn;   			//���ļ���
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	u8 pause=0;			//��ͣ���
	u8 t;
	u16 temp;
	u16 *picindextbl;	//ͼƬ������


/*----------------------���ֳ�ʼ��---------------------------*/
    Stm32_Clock_Init(9);
    delay_init(72);
    uart_init(72,9600);
    LED_Init();
    KEY_Init();
    LCD_Init();


    tp_dev.init();       //��������ʼ��
    mem_init(SRAMIN);   //��ʼ���ڲ��ڴ��
    exf_init();         //Ϊfatfs��ر��������ڴ�
    f_mount(0, fs[0]); //�����ļ�ϵͳ�����빤���ռ�
    while(font_init())
    {
        update_font(16,20,16);
    }
/*-------------------------------------------------------------*/

	Show_Str(60,70,16,0,"ͼƬ��ʾ����",LCD_SIZE_X,16);
	Show_Str(60,90,16,0,"KEY0:NEXT KEY2:PREV", LCD_SIZE_X,16);
	Show_Str(60,130,16,0,"2012��9��19��", LCD_SIZE_X,16);
 	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{
		Show_Str(60,150,16,0,"PICTURE�ļ��д���!", LCD_SIZE_X, 16);
		delay_ms(200);
		LCD_DrawFill(60,150,240,166,WHITE);//�����ʾ
		delay_ms(200);
	}
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0
 	{
		Show_Str(60,150,16,0,"û��ͼƬ�ļ�!", LCD_SIZE_X, 16);
		delay_ms(200);
		LCD_DrawFill(60,150,240,166,WHITE);//�����ʾ
		delay_ms(200);
	}
  	picfileinfo.lfsize = _MAX_LFN*2+1;						//���ļ�����󳤶�
	picfileinfo.lfname = mymalloc(SRAMIN,picfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname = mymalloc(SRAMIN,picfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
 	picindextbl = mymalloc(SRAMIN,2*totpicnum);				//����2*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//�ڴ�������
 	{
		Show_Str(60,150,16,0,"�ڴ����ʧ��!", LCD_SIZE_X, 16);
		delay_ms(200);
		LCD_DrawFill(60,150,240,146,WHITE);//�����ʾ
		delay_ms(200);
	}
	//��¼����
    res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp = picdir.index;								//��¼��ǰindex
	        res = f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo.fname[0]==0)
                break;	//������/��ĩβ��,�˳�
     		fn = (u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);
			res = exf_typetell(fn);
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�
			{
				picindextbl[curindex]=temp;//��¼����
				curindex++;
			}
		}
	}
	Show_Str(60,150,16,0,"��ʼ��ʾ...", LCD_SIZE_X, 16);
	delay_ms(1500);
	piclib_init();										//��ʼ����ͼ
  	curindex=0;											//��0��ʼ��ʾ
   	res = f_opendir(&picdir, (const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{
		dir_sdi(&picdir, picindextbl[curindex]);			//�ı䵱ǰĿ¼����
        res = f_readdir(&picdir, &picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo.fname[0]==0)
            break;	//������/��ĩβ��,�˳�
     	fn = (u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);
		strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0, LCD_SIZE_X, LCD_SIZE_Y );//��ʾͼƬ
		Show_Str(2,2,16,0,pname, LCD_SIZE_X,16);//��ʾͼƬ����
		t=0;
		while(1)
		{
			key=KEY_Scan(0);		//ɨ�谴��
			if(t>250)
                key=KEY1_DOWN;	//ģ��һ�ΰ����Ҽ�
			if(key==KEY2_DOWN)		//��һ��
			{
				if(curindex)
                    curindex--;
				else
                    curindex=totpicnum-1;
				break;
			}
            else if(key==KEY1_DOWN)//��һ��
			{
				curindex++;
				if(curindex>=totpicnum)
                    curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
				break;
			}
            else if(key==KEY3_DOWN)
			{
				pause=!pause;
				LED1=!pause; 	//��ͣ��ʱ��LED1��.
			}
			if(pause==0)
                t++;
			delay_ms(10);
		}
		res=0;
	}
	myfree(SRAMIN,picfileinfo.lfname);	//�ͷ��ڴ�
	myfree(SRAMIN,pname);				//�ͷ��ڴ�
	myfree(SRAMIN,picindextbl);			//�ͷ��ڴ�
}



//��ʾ��Ŀ¼�µ��ļ�
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
    Show_Str(70,0,16,1,"fatfs�ļ����",200,16);

    f_opendir(&dir, "0:");                      //�򿪸�Ŀ¼
    LCD_ShowStr(10,20,16,1, "name             type  size");
    //                       1234567890123456789012345678
    for(cnt=0; ; cnt++)
    {
        res = f_readdir(&dir, fileinfo);
        if(res != FR_OK || fileinfo->fname[0]==0)//Ŀ¼��Ϊ�ջ����
            break;
        if(fileinfo->fname[0]=='.')              //��Ŀ¼(����Ŀ¼) ���Ŀ¼(�ϼ�Ŀ¼)
            continue;
#if _USE_LFN
        fn = *fileinfo->lfname ? fileinfo->lfname : fileinfo->fname;//�ж��Ƿ�Ϊ���ļ���
#else
        fn = fileinfo->fname;
#endif
        LCD_Show10Num(10+22*9, 40+cnt*16, 16,0,3, fileinfo->fsize);//��ʾ�ļ���С
        Show_Str(10,40+cnt*16, 12,0, (u8*)fn, 200,12);//��ʾĿ¼������
        if(fileinfo->fattrib == AM_DIR)
            LCD_ShowStr(10+8*17, 40+cnt*16, 16,0,"dir");//�ļ���
        else
            LCD_ShowStr(10+8*17, 40+cnt*16, 16,0,"file");//�ļ�
    }
#if _USE_LFN
    myfree(SRAMIN, fileinfo->lfname);            //�ͷų��ļ����ڴ�
#endif
	myfree(SRAMIN, fileinfo);
}






//--------------------------------------------
//����USB����
void Load_Draw_Dialog(void)
{
	LCD_Clear(WHITE);//����
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowStr(lcd_dev.width-24,0,16,0,"RST");//��ʾ��������
  	POINT_COLOR=RED;//���û�����ɫ
}
//����x1,x2�ľ���ֵ
u32 usb_abs(u32 x1,u32 x2)
{
	if(x1>x2)return x1-x2;
	else return x2-x1;
}
//����USB ����/����
//enable:0,�Ͽ�
//       1,��������
void usb_port_set(u8 enable)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��
	if(enable)
        _SetCNTR(_GetCNTR()&(~(1<<1)));//�˳��ϵ�ģʽ
	else
	{
		_SetCNTR(_GetCNTR()|(1<<1));  // �ϵ�ģʽ
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;
	}
}

int main(void)
{
	u8 key=0;
	u8 i=0;
 	s8 x0=0;		 //���͵����Զ˵�����ֵ
	s8 y0=0;
	u8 keysta=0;	//[0]:0,����ɿ�;1,�������;
					//[1]:0,�Ҽ��ɿ�;1,�Ҽ�����
					//[2]:0,�м��ɿ�;1,�м�����
	u8 tpsta=0;		//0,��������һ�ΰ���;1,����������
	short xlast=0; //���һ�ΰ��µ�����ֵ
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
 	usb_port_set(0); 	//USB�ȶϿ�
	delay_ms(300);
   	usb_port_set(1);	//USB�ٴ�����
	//USB����
 	USB_Interrupts_Config();
 	Set_USBClock();
 	USB_Init();
	Load_Draw_Dialog();


 	while(1)
	{
		key=KEY_Scan(1);//֧������

		if(key)
		{
			if(key==KEY1_DOWN)
                Joystick_Send(0,0,0,1);		//���͹������ݵ�����
			else if(key==KEY2_DOWN)
                Joystick_Send(0,0,0,(u8)-1);//���͹������ݵ�����
			else
			{
				if(key==KEY3_DOWN)
                    keysta|=0X01;			//����������
				if(key==KEY4_DOWN)
                    keysta|=0X02;			//��������Ҽ�
				Joystick_Send(keysta,0,0,0);//���͸�����
			}
		}
        else if(keysta)//֮ǰ�а���
		{
			keysta=0;
			Joystick_Send(0,0,0,0); //�����ɿ����������
		}



		tp_dev.scan(0);
		if(tp_dev.sta&0x80)//������������
		{
			//�����ƶ�5����λ,���㻬��
  			if( ((usb_abs(tp_dev.x, xlast)>4) || (usb_abs(tp_dev.y,ylast)>4)) &&
                tpsta==0 )//�ȴ�����
			{
				xlast=tp_dev.x;		//��¼�հ��µ�����
				ylast=tp_dev.y;
	 			tpsta=1;
			}
			if(tp_dev.x>216&&tp_dev.y<16)
                Load_Draw_Dialog();//���
			else
                TP_Draw_Big_Point(tp_dev.x,tp_dev.y,RED);		//��ͼ
			if(bDeviceState==CONFIGURED)
			{
				if(tpsta)//����
				{
					x0    = (xlast-tp_dev.x)*3;	//�ϴ�����ֵ��õ�������ֵ֮��,����2��
					y0    = (ylast-tp_dev.y)*3;
					xlast = tp_dev.x;			//��¼�հ��µ�����
					ylast = tp_dev.y;
					Joystick_Send(keysta,-x0,-y0,0); //�������ݵ�����
					delay_ms(5);
				}
			}
		}
        else
		{
			tpsta=0;	//���
 			delay_ms(1);
		}


		if(bDeviceState==CONFIGURED)
            LED1=0;//��USB���óɹ��ˣ�LED1����������
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
