#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "ds18b201.h" 
#include "ds18b202.h" 
#include "ds18b203.h" 
#include "usart.h"
#include "GUI_Control.h"
#include "main.h"         //������Ҫ��ͷ�ļ� 
#include "timer4.h"
#include "usart2.h"     //������Ҫ��ͷ�ļ�
//#include "BC28.h"
#include "usart3.h"     //������Ҫ��ͷ�ļ�
#include "timer23.h"
#include "adc.h"
#include "24cxx.h" 
#include "myiic.h"
 
 LCD_TypeDef lcd;
 u16 Time4_Count;	//1���ʱ��
 u16 Work_Cnt; 
 unsigned char Send_String[25];
 
 int main(void)
 {	
  u16 i;	 
	delay_init();	    	 //��ʱ������ʼ��	  
  LED_Init();		  	 	//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();          	//��ʼ���밴�����ӵ�Ӳ���ӿ�
	AT24CXX_Init();			//IIC��ʼ�� 
 	TIM3_PWM_Init(899,0);//����Ƶ��PWMƵ��=72000/(899+1)=80Khz  
//  TIM2_Cap_Init(0XFFFF,72-1);		//��1Mhz��Ƶ�ʼ���     ��ͷ����ת��
	TIM4_Init(9999,7199);//10Khz�ļ���Ƶ�ʣ�������10000Ϊ1000ms  1��ϵͳ��ʱ��
	uart_init(9600);	 	 //����1��ӡ���� ���������ֻ��������� ����
	 u1_printf("uart1_init ok\r\n");
  Usart3_Init(115200);   //����3��������-ģ�������    ������
   u1_printf("uart3_init ok\r\n");
  Adc_Init();		  		//ADC��ʼ��	 
	 
	while(DS18B20_Init1())	//PCB-DS18B20��ʼ��	
	{
		printf("DS18B20 pcb Error\r\n");
 		delay_ms(200);
	}	
//	while(DS18B20_Init2())	//��Դ-DS18B20��ʼ��	
	{
		printf("DS18B20 power Error\r\n");
 		delay_ms(200);
	}	
  while(DS18B20_Init3())	//��ͷ-DS18B20��ʼ��	
	{
		printf("DS18B20 light Error\r\n");
 		delay_ms(200);
	}	
	
  if(2==KEY_Scan(0))
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);	 //BEEP
		delay_ms(500);
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);	 //BEEP
  	Init_parameter();
	}

//	EEPROM_Data_Write();
  EEPROM_Data_Read();
	 
  Page(1);
//	util_readStm32UniqueCode();  //��STM32����ID��
		lcd.Light_PWM = 0;
	TIM_SetCompare3(TIM3,899);   //PB0--TIM3_CH3  �����PWM  0V
	while(1)
	{ 

			if(1==KEY_Scan(0))
      {  
		    lcd.Light_PWM_state = ~lcd.Light_PWM_state;
				u1_printf("Light_PWM_state = %d\r\n",lcd.Light_PWM_state);  //��ʾ��Ϣ
				
				if(lcd.Light_PWM_state==0)
				{
					LED1=0;

					lcd.DeviceStatus = 0;  // 1����    2��ͣ    3����
					u3_TxData(PIC_page1,10);  // ҳ��
					All_S_time = 0;
					TIM_SetCompare3(TIM3,899);   //PB0--TIM3_CH3  �����PWM  3.3V
				}
				if(lcd.Light_PWM_state!=0)
				{
					lcd.DeviceStatus = 1;
					LED1=1;
					u3_TxData(PIC_page5,10);  //���ƽ��� ҳ��
					All_S_time = 30 * 60;
					up_data();
	        for (i=899;i>0;i--) //450*222 = 99900us  99.900ms
					{
						TIM_SetCompare3(TIM3,i);   //PB0--TIM3_CH3  �򿪵����PWM  0V
//						delay_us(222);
						delay_us(1000);
					}
				}
		  }

			if(Time4_Count%1==0)
			{
				up_data();
			}
	}
}


	u16 Elight_time;	//����ʹ��ʱ��	          0     Elight_time
  u16 Eboard_time;	//���ư�ʹ��ʱ��	        2		  Eboard_time   //�ϵ���ʱ�� ����ͨ��ʱ��
	u16 Elight_pwm;		//����ǿ��PWMƵ��	        4     Elight_pwm
	u16 EBtemp_L;			//���Ӳִ��������¶ȵͣ�	6     EBtemp_L ���ֵ
	u16 EBtemp_M;			//���Ӳִ��������¶��У�	8     EBtemp_M ���ֵ
	u16 EBtemp_H;			//���Ӳִ��������¶ȸߣ�	10		EBtemp_H ���ֵ

  u16 EBtemp_L1;			//���Ӳִ��������¶ȵͣ�	24     EBtemp_L1 ��Сֵ
	u16 EBtemp_M1;			//���Ӳִ��������¶��У�	26     EBtemp_M1 ��Сֵ
	u16 EBtemp_H1;			//���Ӳִ��������¶ȸߣ�	28		EBtemp_H1 ��Сֵ

	u16 ELtemp_L;			//��ѧ�ִ��������¶ȵͣ�	18    ELtemp_L  ���ֵ
	u16 ELtemp_M;			//��ѧ�ִ��������¶��У�	20    ELtemp_M  ���ֵ
	u16 ELtemp_H;			//��ѧ�ִ��������¶ȸߣ�	22    ELtemp_H  ���ֵ

	u16 ELtemp_L1;			//��ѧ�ִ��������¶ȵͣ�	30    ELtemp_L ��Сֵ
	u16 ELtemp_M1;			//��ѧ�ִ��������¶��У�	32    ELtemp_M ��Сֵ
	u16 ELtemp_H1;			//��ѧ�ִ��������¶ȸߣ�	34    ELtemp_H ��Сֵ

  u8 Tim_Num[50]; 	//eeprom��������

void EEPROM_Data_Read(void)
{

	AT24CXX_Read(0,Tim_Num,50);  //eeprom��������
	
	Elight_time = (Tim_Num[0]*256) + (Tim_Num[1]%256);
	Eboard_time = (Tim_Num[2]*256) + (Tim_Num[3]%256);
	Elight_pwm  = (Tim_Num[4]*256) + (Tim_Num[5]%256);
	
	EBtemp_L    = (Tim_Num[6]*256) + (Tim_Num[7]%256);   //���Ӳִ����� �¶� ���ֵ
	EBtemp_M    = (Tim_Num[8]*256) + (Tim_Num[9]%256);   //���Ӳִ����� �¶� ���ֵ
	EBtemp_H    = (Tim_Num[10]*256) + (Tim_Num[11]%256); //���Ӳִ����� �¶� ���ֵ
	
	ELtemp_L    = (Tim_Num[18]*256) + (Tim_Num[19]%256); //��ѧ�ִ����� �¶� ���ֵ
	ELtemp_M		= (Tim_Num[20]*256) + (Tim_Num[21]%256); //��ѧ�ִ����� �¶� ���ֵ
	ELtemp_H		= (Tim_Num[22]*256) + (Tim_Num[23]%256); //��ѧ�ִ����� �¶� ���ֵ
	
  EBtemp_L1   = (Tim_Num[24]*256) + (Tim_Num[25]%256); //���Ӳִ����� �¶� ��Сֵ
	EBtemp_M1   = (Tim_Num[26]*256) + (Tim_Num[27]%256); //���Ӳִ����� �¶� ��Сֵ
	EBtemp_H1   = (Tim_Num[28]*256) + (Tim_Num[29]%256); //���Ӳִ����� �¶� ��Сֵ
	
  ELtemp_L1   = (Tim_Num[30]*256) + (Tim_Num[31]%256); //��ѧ�ִ����� �¶� ��Сֵ
	ELtemp_M1		= (Tim_Num[32]*256) + (Tim_Num[33]%256); //��ѧ�ִ����� �¶� ��Сֵ
	ELtemp_H1		= (Tim_Num[34]*256) + (Tim_Num[35]%256); //��ѧ�ִ����� �¶� ��Сֵ
}

void EEPROM_Data_Write(void)
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////

		Tim_Num[0] =  Elight_time/256;    //����ʹ��ʱ��	          0     Elight_time
		Tim_Num[1] =  Elight_time%256;
		Tim_Num[2] =  Eboard_time/256;    //���ư�ʹ��ʱ��	        2		  Eboard_time
		Tim_Num[3] =  Eboard_time%256;
		Tim_Num[4] =	Elight_pwm/256;		//����ǿ��PWMƵ��	        4     Elight_pwm
		Tim_Num[5] =  Elight_pwm%256;
	
		Tim_Num[6] =	EBtemp_L/256;		//���Ӳִ��������¶ȵͣ�	6     EBtemp_L
		Tim_Num[7] =  EBtemp_L%256;
		Tim_Num[8] =	EBtemp_M/256;		//���Ӳִ��������¶��У�	8     EBtemp_M
		Tim_Num[9] =  EBtemp_M%256;
		Tim_Num[10] =	EBtemp_H/256;		//���Ӳִ��������¶ȸߣ�	10		EBtemp_H
		Tim_Num[11] = EBtemp_H%256;

		Tim_Num[18] =	ELtemp_L/256;		//��ѧ�ִ��������¶ȵͣ�	18    ELtemp_L
		Tim_Num[19] = ELtemp_L%256;
		Tim_Num[20] =	ELtemp_M/256;		//��ѧ�ִ��������¶��У�	20    ELtemp_M
		Tim_Num[21] = ELtemp_M%256;
		Tim_Num[22] =	ELtemp_H/256;		//��ѧ�ִ��������¶ȸߣ�	22    ELtemp_H
		Tim_Num[23] = ELtemp_H%256;
		
	  Tim_Num[24] =	EBtemp_L1/256;		//���Ӳִ��������¶ȵͣ�	6     EBtemp_L
		Tim_Num[25] = EBtemp_L1%256;
		Tim_Num[26] =	EBtemp_M1/256;		//���Ӳִ��������¶��У�	8     EBtemp_M
		Tim_Num[27] = EBtemp_M1%256;
		Tim_Num[28] =	EBtemp_H1/256;		//���Ӳִ��������¶ȸߣ�	10		EBtemp_H
		Tim_Num[29] = EBtemp_H1%256;

		Tim_Num[30] =	ELtemp_L1/256;		//��ѧ�ִ��������¶ȵͣ�	18    ELtemp_L
		Tim_Num[31] = ELtemp_L1%256;
		Tim_Num[32] =	ELtemp_M1/256;		//��ѧ�ִ��������¶��У�	20    ELtemp_M
		Tim_Num[33] = ELtemp_M1%256;
		Tim_Num[34] =	ELtemp_H1/256;		//��ѧ�ִ��������¶ȸߣ�	22    ELtemp_H
		Tim_Num[35] = ELtemp_H1%256;
		
		
		AT24CXX_Write(0, Tim_Num,50);
}

void Init_parameter(void)
{
	Elight_time = 0;
	Eboard_time = 0;
	Elight_pwm  = 89;
 
	EBtemp_L1 = 1;  //���Ӳִ����� �¶� ��Сֵ
	EBtemp_M1 = 30;  //���Ӳִ����� �¶� ��Сֵ
	EBtemp_H1 = 60;  //���Ӳִ����� �¶� ��Сֵ
	
	EBtemp_L = 30;  //���Ӳִ��������¶� ���ֵ
	EBtemp_M = 60;   //���Ӳִ��������¶� ���ֵ
	EBtemp_H = 99;   //���Ӳִ��������¶� ���ֵ
	
	ELtemp_L1 = 1;  //��ѧ�ִ����� �¶� ��Сֵ
	ELtemp_M1 = 30;  //��ѧ�ִ����� �¶� ��Сֵ
	ELtemp_H1 = 60;  //��ѧ�ִ����� �¶� ��Сֵ
	
	ELtemp_L = 30;   //��ѧ�ִ����� �¶� ���ֵ
	ELtemp_M = 60;   //��ѧ�ִ����� �¶� ���ֵ
	ELtemp_H = 99;   //��ѧ�ִ����� �¶� ���ֵ
           
		Tim_Num[0] =  Elight_time/256;    //����ʹ��ʱ��	          0     Elight_time
		Tim_Num[1] =  Elight_time%256;
		Tim_Num[2] =  Eboard_time/256;    //���ư�ʹ��ʱ��	        2		  Eboard_time
		Tim_Num[3] =  Eboard_time%256;
		Tim_Num[4] =	Elight_pwm/256;		//����ǿ��PWMƵ��	        4     Elight_pwm
		Tim_Num[5] =  Elight_pwm%256;
	
		Tim_Num[6] =	EBtemp_L/256;		//���Ӳִ��������¶ȵͣ�	6     EBtemp_L
		Tim_Num[7] =  EBtemp_L%256;
		Tim_Num[8] =	EBtemp_M/256;		//���Ӳִ��������¶��У�	8     EBtemp_M
		Tim_Num[9] =  EBtemp_M%256;
		Tim_Num[10] =	EBtemp_H/256;		//���Ӳִ��������¶ȸߣ�	10		EBtemp_H
		Tim_Num[11] = EBtemp_H%256;

		Tim_Num[18] =	ELtemp_L/256;		//��ѧ�ִ��������¶ȵͣ�	18    ELtemp_L
		Tim_Num[19] = ELtemp_L%256;
		Tim_Num[20] =	ELtemp_M/256;		//��ѧ�ִ��������¶��У�	20    ELtemp_M
		Tim_Num[21] = ELtemp_M%256;
		Tim_Num[22] =	ELtemp_H/256;		//��ѧ�ִ��������¶ȸߣ�	22    ELtemp_H
		Tim_Num[23] = ELtemp_H%256;
		
	  Tim_Num[24] =	EBtemp_L1/256;		//���Ӳִ��������¶ȵͣ�	6     EBtemp_L
		Tim_Num[25] = EBtemp_L1%256;
		Tim_Num[26] =	EBtemp_M1/256;		//���Ӳִ��������¶��У�	8     EBtemp_M
		Tim_Num[27] = EBtemp_M1%256;
		Tim_Num[28] =	EBtemp_H1/256;		//���Ӳִ��������¶ȸߣ�	10		EBtemp_H
		Tim_Num[29] = EBtemp_H1%256;

		Tim_Num[30] =	ELtemp_L1/256;		//��ѧ�ִ��������¶ȵͣ�	18    ELtemp_L
		Tim_Num[31] = ELtemp_L1%256;
		Tim_Num[32] =	ELtemp_M1/256;		//��ѧ�ִ��������¶��У�	20    ELtemp_M
		Tim_Num[33] = ELtemp_M1%256;
		Tim_Num[34] =	ELtemp_H1/256;		//��ѧ�ִ��������¶ȸߣ�	22    ELtemp_H
		Tim_Num[35] = ELtemp_H1%256;

		AT24CXX_Write(0, Tim_Num,50);
}



