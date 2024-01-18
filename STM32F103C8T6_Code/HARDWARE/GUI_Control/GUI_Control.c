#include "GUI_Control.h"
#include "sys.h"
#include "delay.h"	 
#include "lcd.h"  
#include "stdio.h"
#include "main.h"         //������Ҫ��ͷ�ļ�
#include "ds18b201.h" 
#include "ds18b202.h" 
#include "ds18b203.h" 
#include <string.h>
#include "led.h"
#include "usart.h"	
#include "usart3.h"     //������Ҫ��ͷ�ļ�
#include "24cxx.h" 

//u8 g_stm32_uniqueId[12] = {0};

//void util_readStm32UniqueCode(void)   //��STM32����ID��
//{
//    u8 v;
//		vu8 * addr = (vu8*)(0x1FFFF7E8);
//		u8 i;
//		for(i=0; i<12;i++)
//	  {
//			v = *addr;
//			g_stm32_uniqueId[i] = v;
//			++addr;
//		}
//}


void sort(int a[],int n)
{
	int i,j,temp;
	for(i=0;i<n-1;i++)                   //�Ƚ�n-1�Σ���һ��ѭ����ʾ������
	{   
		for(j=0;j<n-i-1;j++)              // ���һ�αȽ�a[n-i-1]��a[n-i-2]   ���ڶ���ѭ����ʾ�Ƚϴ��� ��
		{   
			if(a[j]<a[j+1])
			{   
				temp = a[j+1];
				a[j+1] = a[j];
				a[j] = temp;
			}
		}
	}
}

u16 Get_Fan_Speed2(u16 pwm)  //pwm 0-899   ��Դ�ַ��� CON2
{
	float speed2;
  TIM_SetCompare1(TIM3,899-pwm);   //PA6--TIM3_CH1  ��Դ�ַ��� CON2
	speed2 = pwm * 3000 /899 ;
	return speed2;
}

//extern u8  TIM2CH1_CAPTURE_STA;		//���벶��״̬		    				
//extern u16	TIM2CH1_CAPTURE_VAL;	//���벶��ֵ
u16 Get_Fan_Speed(u16 pwm)  //pwm 0-899   ���ݷ���
{
	float speed;
  TIM_SetCompare2(TIM3,899-pwm);   //PA7--TIM3_CH2  ���ݷ��� CON3
	speed = pwm * 3000 /899 ;
	return speed;
}

unsigned char PIC_page0[10]={0x5A,0xA5,0x07,0x82,0x00,0x84,0x5A,0x01,0x00,0x00};
unsigned char PIC_page1[10]={0x5A,0xA5,0x07,0x82,0x00,0x84,0x5A,0x01,0x00,0x01};
unsigned char PIC_page5[10]={0x5A,0xA5,0x07,0x82,0x00,0x84,0x5A,0x01,0x00,0x05};
unsigned char PIC_page7[10]={0x5A,0xA5,0x07,0x82,0x00,0x84,0x5A,0x01,0x00,0x07}; //����
  unsigned char lcd_data[]={
	0x5a ,0xa5 ,0x05 ,0x82 ,0x10 ,0x10 ,
	0x00 ,0x00};

void Page(char cnt)
{
	lcd.DeviceStatus = 0;   // 1����    2��ͣ    3����
	lcd.Surplus_min  = 0;  //��  ʣ��ʹ��ʱ��
	lcd.Surplus_sec  = 0;  //��  ʣ��ʹ��ʱ�� 

	if(cnt == 1)
	 u3_TxData(PIC_page1,10);
}

void get_ds18b20_data(void)
{
  lcd.Light_T=DS18B20_Get_Temp3();  // ��ͷ�¶Ȼ�ȡ
	lcd_data[5] = 0x10;
	lcd_data[6] = (lcd.Light_T>>8)&0x00ff;
	lcd_data[7] =  lcd.Light_T    &0x00ff;
	u3_TxData(lcd_data,8);
 
  lcd.Power_T=DS18B20_Get_Temp2();  // ��Դ�¶Ȼ�ȡ
	lcd_data[5] = 0x21;
	lcd_data[6] = (lcd.Power_T>>8)&0x00ff;
	lcd_data[7] =  lcd.Power_T    &0x00ff;
	u3_TxData(lcd_data,8); 

  lcd.Board_T=DS18B20_Get_Temp1();  // PCB�¶Ȼ�ȡ
	lcd_data[5] = 0x20;
	lcd_data[6] = (lcd.Board_T>>8)&0x00ff;
	lcd_data[7] =  lcd.Board_T    &0x00ff;
	u3_TxData(lcd_data,8);
}

void Fan_Speed_Ctrl(void)
{
	  EEPROM_Data_Read();
    if(ELtemp_L1 < lcd.Light_T <= ELtemp_L)  //С�� ELtemp_L �����50%PWM
		{
			Get_Fan_Speed(890*0.5);  //pwm 0-899
			lcd.Light_FAN = 1000;
			lcd_data[5] = 0x11; 
			lcd_data[6] = (lcd.Light_FAN>>8)&0x00ff;
			lcd_data[7] =  lcd.Light_FAN    &0x00ff;
			u3_TxData(lcd_data,8); delay_ms(10);
			u1_printf("---��ѧ�ַ��ٿ���: ELtemp_L=1000\r\n");
		}
		else if(ELtemp_M1*10 < lcd.Light_T <= ELtemp_M*10)  //ELtemp_L �� ~ ELtemp_M �棬80%PWM
		{
			Get_Fan_Speed(890*0.8);  //pwm 0-899
			lcd.Light_FAN = 2000;
			lcd_data[5] = 0x11; 
			lcd_data[6] = (lcd.Light_FAN>>8)&0x00ff;
			lcd_data[7] =  lcd.Light_FAN    &0x00ff;
			u3_TxData(lcd_data,8); delay_ms(10);
			u1_printf("---��ѧ�ַ��ٿ���: ELtemp_M=2000\r\n");
		}
		else if(ELtemp_H1*10 < lcd.Light_T <= ELtemp_H*10)  //ELtemp_M �� ~ ELtemp_H �棬100%PWM
		{
			Get_Fan_Speed(890);  //pwm 0-899
			lcd.Light_FAN = 3000;
			lcd_data[5] = 0x11; 
			lcd_data[6] = (lcd.Light_FAN>>8)&0x00ff;
			lcd_data[7] =  lcd.Light_FAN    &0x00ff;
			u3_TxData(lcd_data,8); delay_ms(10);
			u1_printf("---��ѧ�ַ��ٿ���: ELtemp_H=3000\r\n");
		}
//		else 
//  ��Դ���ٿ���
		if(EBtemp_L1*10 < lcd.Power_T <= EBtemp_L*10)
		{
			Get_Fan_Speed2(890*0.5);  //pwm 0-899 ��Դ�ַ���
			lcd.Power_FAN = 1000;
			lcd_data[5] = 0x22; 
			lcd_data[6] = (lcd.Power_FAN>>8)&0x00ff;
			lcd_data[7] =  lcd.Power_FAN    &0x00ff;
			u3_TxData(lcd_data,8); delay_ms(10);
			u1_printf("------��Դ���ٿ���:   EBtemp_L 1000  \r\n" ); 
		}
		else if(EBtemp_M1*10 < lcd.Power_T <= EBtemp_M*10)
		{
			Get_Fan_Speed2(890*0.8);  //pwm 0-899 ��Դ�ַ���
			lcd.Power_FAN = 2000;
			lcd_data[5] = 0x22; 
			lcd_data[6] = (lcd.Power_FAN>>8)&0x00ff;
			lcd_data[7] =  lcd.Power_FAN    &0x00ff;
			u3_TxData(lcd_data,8); delay_ms(10);
			u1_printf("------��Դ���ٿ���:   EBtemp_M 2000 \r\n" ); 
		}
		else if(EBtemp_H1*10 < lcd.Power_T <= EBtemp_H*10)
		{
			Get_Fan_Speed2(890);  //pwm 0-899 ��Դ�ַ���
			lcd.Power_FAN = 3000;
			lcd_data[5] = 0x22; 
			lcd_data[6] = (lcd.Power_FAN>>8)&0x00ff;
			lcd_data[7] =  lcd.Power_FAN    &0x00ff;
			u3_TxData(lcd_data,8); delay_ms(10);
			u1_printf("------��Դ���ٿ���:   EBtemp_H 3000 \r\n" ); 
		} 
//		u1_printf("lcd.Light_T: %d lcd.Power_T = %d lcd.Light_FAN=%d   lcd.Power_FAN: %d  \r\n",lcd.Light_T,lcd.Power_T,lcd.Light_FAN,lcd.Power_FAN);
}

u16 Light_PWM_base;

void Light_PWM(u16 L_PWM)
{
	  u16 i;  
	float light_pwm;
	  lcd.Light_PWM = L_PWM;
		switch(lcd.Light_PWM)
		{
			case 50: // 50%����  32           899��0v
				 EEPROM_Data_Read();
			   light_pwm = Elight_pwm * 0.01;  //    90 / 100 = 0.90  Elight_pwm
//			u1_printf("Elight_pwm: %d light_pw:%3.2f \r\n",Elight_pwm,light_pw);
//			  for (i=400;i<(899*light_pwm);i++) //3s  90%  899*0.9=809  899*light_pwm
//			  for (i=899;i>(899-(899*light_pwm));i--) //3s  90%  899*0.9=809  899*light_pwm
//			  {
//			    TIM_SetCompare3(TIM3,i);   //PB0--TIM3_CH3  �����PWM 720
//					delay_us(222);
//			  }
				TIM_SetCompare3(TIM3,899-(899*light_pwm));   //PB0--TIM3_CH3  �����PWM 720
				Light_PWM_base = lcd.Light_PWM;
        LED1=1;  lcd.Light_PWM_state=255;
				u1_printf("lcd.Light_PWM: %d \r\n",lcd.Light_PWM);
			break;

			case 100://�������   64
				TIM_SetCompare3(TIM3,0);   //PB0--TIM3_CH3  �򿪵����PWM  0V
				Light_PWM_base = lcd.Light_PWM;
			  LED1=1;	 lcd.Light_PWM_state=255;
				u1_printf("lcd.Light_PWM: %d  \r\n",lcd.Light_PWM);
			break;
			
			case 101://�������   64
//				for (i=400;i<899;i++) //3s
			  for (i=899;i>0;i--) //3s
			  {
			    TIM_SetCompare3(TIM3,i);   //PB0--TIM3_CH3  �򿪵����PWM  0V
//					delay_us(222);
					delay_us(1000);
			  }
				Light_PWM_base = lcd.Light_PWM;
			  LED1=1;	 lcd.Light_PWM_state=255;
				u1_printf("lcd.Light_PWM: %d  \r\n",lcd.Light_PWM);
			break;
						
			case 0:   //ֹͣ
				lcd.Set_time = 0;
			  LED1=0;  lcd.Light_PWM_state=0;
				TIM_SetCompare3(TIM3,899);   //PB0--TIM3_CH3  �����PWM 0  3.3V /// 899  0V
//						TIM_SetCompare3(TIM3,899-(899*0.7));   //PB0--TIM3_CH3  �����PWM
				Light_PWM_base = lcd.Light_PWM;
				u1_printf("lcd.Light_PWM: %d  \r\n",lcd.Light_PWM);
				u3_TxData(PIC_page5,10);  //���ƽ��� ҳ��
			break;
								
			default:  break;
		}
}



	unsigned char lcd_Cdata1[]= {  //��������
	 0x5a ,0xa5 ,0x0b ,0x82 ,0x10 ,0x30 ,
//	 0xB9 ,0xA4 ,0xD7 ,0xF7 ,
	0xD5 ,0xFD ,0xB3 ,0xA3};
	unsigned char lcd_Cdata2[]= {  //��ͣ����
	 0x5a ,0xa5 ,0x0b ,0x82 ,0x10 ,0x30 ,
	 0xBD ,0xE1 ,0xCA ,0xF8//0xD4 ,0xDD ,0xCD ,0xA3 ,
	 //0xB9 ,0xA4 ,0xD7 ,0xF7
	};  
	
//	unsigned char lcd_Cdata3[]= {  //���ƽ���
//	 0x5a ,0xa5 ,0x0b ,0x82 ,0x10 ,0x30 ,
//	 0xB9 ,0xE2 ,0xC1 ,0xC6 ,
//	 0xBD ,0xE1 ,0xCA ,0xF8};
	
  unsigned char Max_Time[]={
	0x5a ,0xa5 ,0x05 ,0x82 ,0x10 ,0x14 ,
	0x00 ,0x1e};
	
u8 cont=1;
u16 Work_timer_C;
u16 All_S_time;      //���õ�������
//u8 board_time=0;   //ͨ��ʱ��
u8 light_time=0;   //����ʹ��ʱ��
void up_data(void)
{

	u16 Surp_time;      //ʣ��������
  
	
	if(lcd.Set_time>30 || lcd.Set_time==90) //���ù����30����
	{
	 lcd.Set_time = 30;
	 u3_TxData(Max_Time,8);
	}
				 
		lcd.Work_timer = Time4_Count/73;
    if(lcd.Work_timer != Work_timer_C)
		{ 
			Work_timer_C = lcd.Work_timer;
		  EEPROM_Data_Read();
      Eboard_time += 1;
			Tim_Num[2] = (Eboard_time>>8)&0x00ff;
			Tim_Num[3] =  Eboard_time    &0x00ff;
			AT24CXX_WriteOneByte(2,Tim_Num[2]);  //���ư�ʹ��ʱ��	        2		  Eboard_time
			AT24CXX_WriteOneByte(3,Tim_Num[3]);
			u1_printf("Eboard_time: %d  \r\n",Eboard_time);
			
		}
// u1_printf("lcd.DeviceStatus: %d  \r\n",lcd.DeviceStatus);
	if(lcd.DeviceStatus == 1)  // 1����  
	{
	  Surp_time = All_S_time - Work_Cnt;
		if(Surp_time==1) //���ʣ��������==���л�������ҳ��
		{
			LED1=0;
			lcd.DeviceStatus=3;   //  3����
		}
//     u1_printf("Surp_time: %d  \r\n",Surp_time);
		if(Surp_time!=0)
		{
			lcd.Surplus_min = Surp_time/60;
			lcd.Surplus_sec = Surp_time%60;

			lcd_data[5] = 0x12;   //ʣ��ʱ����
			lcd_data[6] = (lcd.Surplus_min>>8)&0x00ff;
			lcd_data[7] =  lcd.Surplus_min    &0x00ff;
			u3_TxData(lcd_data,8);

			lcd_data[5] = 0x13;   //ʣ��ʱ����
			lcd_data[6] = (lcd.Surplus_sec>>8)&0x00ff;
			lcd_data[7] =  lcd.Surplus_sec    &0x00ff;
			u3_TxData(lcd_data,8);

			if(lcd.Light_PWM != Light_PWM_base)
			{
		    Light_PWM(lcd.Light_PWM);
			}
		}
		u3_TxData(lcd_Cdata1,14);  //��������
	}
 
	if(lcd.DeviceStatus==3)      // 3����
	{
		lcd.DeviceStatus = 0;   // 1����    2��ͣ    3����
		u3_TxData(PIC_page7,10);  //���ƽ��� ҳ��
		u3_TxData(lcd_Cdata2,14);  //��ͣ����
		TIM_SetCompare3(TIM3,899);   //PB0--TIM3_CH3   �رյ����PWM  3.3V
		
		EEPROM_Data_Read();
		
 	  light_time = Elight_time + lcd.Set_time; //eeprom�������� ��ʹ�÷�����
 	  Tim_Num[0] =  light_time/256;    //����ʹ��ʱ��	          0     Elight_time
 	  Tim_Num[1] =  light_time%256;   //���汾��ʹ��ʱ����EEPROM��
		AT24CXX_WriteOneByte(0,Tim_Num[0]);
		AT24CXX_WriteOneByte(1,Tim_Num[1]);
		u1_printf("�����ʹ��ʱ����EEPROM�� light_time: %d  \r\n",light_time);
	}
	
	get_ds18b20_data(); //��ȡ�������¶ȴ�����
 
	//����¶�������ٿ���
	cont--;
	if(Time4_Count%5==0 && cont==0)
	{
		Fan_Speed_Ctrl();
		cont = 1;
		delay_ms(200);
		
	}
	if(Time4_Count%100==0 && lcd.DeviceStatus==0)   //��ʱ������10�������ͼƬ
	{
		u3_TxData(PIC_page0,10);
	}
}







