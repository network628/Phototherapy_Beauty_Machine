#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "ds18b201.h" 
#include "ds18b202.h" 
#include "ds18b203.h" 
#include "usart.h"
#include "GUI_Control.h"
#include "main.h"         //包含需要的头文件 
#include "timer4.h"
#include "usart2.h"     //包含需要的头文件
//#include "BC28.h"
#include "usart3.h"     //包含需要的头文件
#include "timer23.h"
#include "adc.h"
#include "24cxx.h" 
#include "myiic.h"
 
 LCD_TypeDef lcd;
 u16 Time4_Count;	//1秒计时器
 u16 Work_Cnt; 
 unsigned char Send_String[25];
 
 int main(void)
 {	
  u16 i;	 
	delay_init();	    	 //延时函数初始化	  
  LED_Init();		  	 	//初始化与LED连接的硬件接口
	KEY_Init();          	//初始化与按键连接的硬件接口
	AT24CXX_Init();			//IIC初始化 
 	TIM3_PWM_Init(899,0);//不分频。PWM频率=72000/(899+1)=80Khz  
//  TIM2_Cap_Init(0XFFFF,72-1);		//以1Mhz的频率计数     灯头风扇转速
	TIM4_Init(9999,7199);//10Khz的计数频率，计数到10000为1000ms  1秒系统计时器
	uart_init(9600);	 	 //串口1打印电脑 （串口助手或者蓝牙） 数据
	 u1_printf("uart1_init ok\r\n");
  Usart3_Init(115200);   //串口3接收体重-模块的数据    串口屏
   u1_printf("uart3_init ok\r\n");
  Adc_Init();		  		//ADC初始化	 
	 
	while(DS18B20_Init1())	//PCB-DS18B20初始化	
	{
		printf("DS18B20 pcb Error\r\n");
 		delay_ms(200);
	}	
//	while(DS18B20_Init2())	//电源-DS18B20初始化	
	{
		printf("DS18B20 power Error\r\n");
 		delay_ms(200);
	}	
  while(DS18B20_Init3())	//灯头-DS18B20初始化	
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
//	util_readStm32UniqueCode();  //读STM32——ID号
		lcd.Light_PWM = 0;
	TIM_SetCompare3(TIM3,899);   //PB0--TIM3_CH3  电灯泡PWM  0V
	while(1)
	{ 

			if(1==KEY_Scan(0))
      {  
		    lcd.Light_PWM_state = ~lcd.Light_PWM_state;
				u1_printf("Light_PWM_state = %d\r\n",lcd.Light_PWM_state);  //提示信息
				
				if(lcd.Light_PWM_state==0)
				{
					LED1=0;

					lcd.DeviceStatus = 0;  // 1工作    2暂停    3结束
					u3_TxData(PIC_page1,10);  // 页面
					All_S_time = 0;
					TIM_SetCompare3(TIM3,899);   //PB0--TIM3_CH3  电灯泡PWM  3.3V
				}
				if(lcd.Light_PWM_state!=0)
				{
					lcd.DeviceStatus = 1;
					LED1=1;
					u3_TxData(PIC_page5,10);  //光疗结束 页面
					All_S_time = 30 * 60;
					up_data();
	        for (i=899;i>0;i--) //450*222 = 99900us  99.900ms
					{
						TIM_SetCompare3(TIM3,i);   //PB0--TIM3_CH3  打开电灯泡PWM  0V
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


	u16 Elight_time;	//灯泡使用时长	          0     Elight_time
  u16 Eboard_time;	//控制板使用时长	        2		  Eboard_time   //上电总时长 整机通电时长
	u16 Elight_pwm;		//光照强度PWM频率	        4     Elight_pwm
	u16 EBtemp_L;			//电子仓传感器（温度低）	6     EBtemp_L 最大值
	u16 EBtemp_M;			//电子仓传感器（温度中）	8     EBtemp_M 最大值
	u16 EBtemp_H;			//电子仓传感器（温度高）	10		EBtemp_H 最大值

  u16 EBtemp_L1;			//电子仓传感器（温度低）	24     EBtemp_L1 最小值
	u16 EBtemp_M1;			//电子仓传感器（温度中）	26     EBtemp_M1 最小值
	u16 EBtemp_H1;			//电子仓传感器（温度高）	28		EBtemp_H1 最小值

	u16 ELtemp_L;			//光学仓传感器（温度低）	18    ELtemp_L  最大值
	u16 ELtemp_M;			//光学仓传感器（温度中）	20    ELtemp_M  最大值
	u16 ELtemp_H;			//光学仓传感器（温度高）	22    ELtemp_H  最大值

	u16 ELtemp_L1;			//光学仓传感器（温度低）	30    ELtemp_L 最小值
	u16 ELtemp_M1;			//光学仓传感器（温度中）	32    ELtemp_M 最小值
	u16 ELtemp_H1;			//光学仓传感器（温度高）	34    ELtemp_H 最小值

  u8 Tim_Num[50]; 	//eeprom缓存数组

void EEPROM_Data_Read(void)
{

	AT24CXX_Read(0,Tim_Num,50);  //eeprom缓存数组
	
	Elight_time = (Tim_Num[0]*256) + (Tim_Num[1]%256);
	Eboard_time = (Tim_Num[2]*256) + (Tim_Num[3]%256);
	Elight_pwm  = (Tim_Num[4]*256) + (Tim_Num[5]%256);
	
	EBtemp_L    = (Tim_Num[6]*256) + (Tim_Num[7]%256);   //电子仓传感器 温度 最大值
	EBtemp_M    = (Tim_Num[8]*256) + (Tim_Num[9]%256);   //电子仓传感器 温度 最大值
	EBtemp_H    = (Tim_Num[10]*256) + (Tim_Num[11]%256); //电子仓传感器 温度 最大值
	
	ELtemp_L    = (Tim_Num[18]*256) + (Tim_Num[19]%256); //光学仓传感器 温度 最大值
	ELtemp_M		= (Tim_Num[20]*256) + (Tim_Num[21]%256); //光学仓传感器 温度 最大值
	ELtemp_H		= (Tim_Num[22]*256) + (Tim_Num[23]%256); //光学仓传感器 温度 最大值
	
  EBtemp_L1   = (Tim_Num[24]*256) + (Tim_Num[25]%256); //电子仓传感器 温度 最小值
	EBtemp_M1   = (Tim_Num[26]*256) + (Tim_Num[27]%256); //电子仓传感器 温度 最小值
	EBtemp_H1   = (Tim_Num[28]*256) + (Tim_Num[29]%256); //电子仓传感器 温度 最小值
	
  ELtemp_L1   = (Tim_Num[30]*256) + (Tim_Num[31]%256); //光学仓传感器 温度 最小值
	ELtemp_M1		= (Tim_Num[32]*256) + (Tim_Num[33]%256); //光学仓传感器 温度 最小值
	ELtemp_H1		= (Tim_Num[34]*256) + (Tim_Num[35]%256); //光学仓传感器 温度 最小值
}

void EEPROM_Data_Write(void)
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////

		Tim_Num[0] =  Elight_time/256;    //灯泡使用时长	          0     Elight_time
		Tim_Num[1] =  Elight_time%256;
		Tim_Num[2] =  Eboard_time/256;    //控制板使用时长	        2		  Eboard_time
		Tim_Num[3] =  Eboard_time%256;
		Tim_Num[4] =	Elight_pwm/256;		//光照强度PWM频率	        4     Elight_pwm
		Tim_Num[5] =  Elight_pwm%256;
	
		Tim_Num[6] =	EBtemp_L/256;		//电子仓传感器（温度低）	6     EBtemp_L
		Tim_Num[7] =  EBtemp_L%256;
		Tim_Num[8] =	EBtemp_M/256;		//电子仓传感器（温度中）	8     EBtemp_M
		Tim_Num[9] =  EBtemp_M%256;
		Tim_Num[10] =	EBtemp_H/256;		//电子仓传感器（温度高）	10		EBtemp_H
		Tim_Num[11] = EBtemp_H%256;

		Tim_Num[18] =	ELtemp_L/256;		//光学仓传感器（温度低）	18    ELtemp_L
		Tim_Num[19] = ELtemp_L%256;
		Tim_Num[20] =	ELtemp_M/256;		//光学仓传感器（温度中）	20    ELtemp_M
		Tim_Num[21] = ELtemp_M%256;
		Tim_Num[22] =	ELtemp_H/256;		//光学仓传感器（温度高）	22    ELtemp_H
		Tim_Num[23] = ELtemp_H%256;
		
	  Tim_Num[24] =	EBtemp_L1/256;		//电子仓传感器（温度低）	6     EBtemp_L
		Tim_Num[25] = EBtemp_L1%256;
		Tim_Num[26] =	EBtemp_M1/256;		//电子仓传感器（温度中）	8     EBtemp_M
		Tim_Num[27] = EBtemp_M1%256;
		Tim_Num[28] =	EBtemp_H1/256;		//电子仓传感器（温度高）	10		EBtemp_H
		Tim_Num[29] = EBtemp_H1%256;

		Tim_Num[30] =	ELtemp_L1/256;		//光学仓传感器（温度低）	18    ELtemp_L
		Tim_Num[31] = ELtemp_L1%256;
		Tim_Num[32] =	ELtemp_M1/256;		//光学仓传感器（温度中）	20    ELtemp_M
		Tim_Num[33] = ELtemp_M1%256;
		Tim_Num[34] =	ELtemp_H1/256;		//光学仓传感器（温度高）	22    ELtemp_H
		Tim_Num[35] = ELtemp_H1%256;
		
		
		AT24CXX_Write(0, Tim_Num,50);
}

void Init_parameter(void)
{
	Elight_time = 0;
	Eboard_time = 0;
	Elight_pwm  = 89;
 
	EBtemp_L1 = 1;  //电子仓传感器 温度 最小值
	EBtemp_M1 = 30;  //电子仓传感器 温度 最小值
	EBtemp_H1 = 60;  //电子仓传感器 温度 最小值
	
	EBtemp_L = 30;  //电子仓传感器（温度 最大值
	EBtemp_M = 60;   //电子仓传感器（温度 最大值
	EBtemp_H = 99;   //电子仓传感器（温度 最大值
	
	ELtemp_L1 = 1;  //光学仓传感器 温度 最小值
	ELtemp_M1 = 30;  //光学仓传感器 温度 最小值
	ELtemp_H1 = 60;  //光学仓传感器 温度 最小值
	
	ELtemp_L = 30;   //光学仓传感器 温度 最大值
	ELtemp_M = 60;   //光学仓传感器 温度 最大值
	ELtemp_H = 99;   //光学仓传感器 温度 最大值
           
		Tim_Num[0] =  Elight_time/256;    //灯泡使用时长	          0     Elight_time
		Tim_Num[1] =  Elight_time%256;
		Tim_Num[2] =  Eboard_time/256;    //控制板使用时长	        2		  Eboard_time
		Tim_Num[3] =  Eboard_time%256;
		Tim_Num[4] =	Elight_pwm/256;		//光照强度PWM频率	        4     Elight_pwm
		Tim_Num[5] =  Elight_pwm%256;
	
		Tim_Num[6] =	EBtemp_L/256;		//电子仓传感器（温度低）	6     EBtemp_L
		Tim_Num[7] =  EBtemp_L%256;
		Tim_Num[8] =	EBtemp_M/256;		//电子仓传感器（温度中）	8     EBtemp_M
		Tim_Num[9] =  EBtemp_M%256;
		Tim_Num[10] =	EBtemp_H/256;		//电子仓传感器（温度高）	10		EBtemp_H
		Tim_Num[11] = EBtemp_H%256;

		Tim_Num[18] =	ELtemp_L/256;		//光学仓传感器（温度低）	18    ELtemp_L
		Tim_Num[19] = ELtemp_L%256;
		Tim_Num[20] =	ELtemp_M/256;		//光学仓传感器（温度中）	20    ELtemp_M
		Tim_Num[21] = ELtemp_M%256;
		Tim_Num[22] =	ELtemp_H/256;		//光学仓传感器（温度高）	22    ELtemp_H
		Tim_Num[23] = ELtemp_H%256;
		
	  Tim_Num[24] =	EBtemp_L1/256;		//电子仓传感器（温度低）	6     EBtemp_L
		Tim_Num[25] = EBtemp_L1%256;
		Tim_Num[26] =	EBtemp_M1/256;		//电子仓传感器（温度中）	8     EBtemp_M
		Tim_Num[27] = EBtemp_M1%256;
		Tim_Num[28] =	EBtemp_H1/256;		//电子仓传感器（温度高）	10		EBtemp_H
		Tim_Num[29] = EBtemp_H1%256;

		Tim_Num[30] =	ELtemp_L1/256;		//光学仓传感器（温度低）	18    ELtemp_L
		Tim_Num[31] = ELtemp_L1%256;
		Tim_Num[32] =	ELtemp_M1/256;		//光学仓传感器（温度中）	20    ELtemp_M
		Tim_Num[33] = ELtemp_M1%256;
		Tim_Num[34] =	ELtemp_H1/256;		//光学仓传感器（温度高）	22    ELtemp_H
		Tim_Num[35] = ELtemp_H1%256;

		AT24CXX_Write(0, Tim_Num,50);
}



