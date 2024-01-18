#include "led.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PD端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //BEEP
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PA.8 输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;	    		 //TG1  LED 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	GPIO_ResetBits(GPIOC,GPIO_Pin_11 | GPIO_Pin_12); 						 //PD.2 输出高 

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;//PC10  BUSY
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化 
}
 
 

/*--------------------------------------
;模块名称:Line_1A_WTN6(UC8 DDATA)
;功能:实现一线串口通信函数
;入参: DDATA 为发送数据
;出参:
;P_DATA 为数据口
;-------------------------------------*/
void Line_1A_WTN6(u8 DDATA)
{
	u8 S_DATA,j;
	char B_DATA;
	S_DATA = DDATA;
	P_DATA = 0;
	delay_ms(500); //延时5ms
	B_DATA = S_DATA&0X01;
	for(j=0;j<8;j++)
	{
		if(B_DATA == 1)
		{
			P_DATA = 1;
			delay_us(600); //延时600us
			
			P_DATA = 0;
			delay_us(200); //延时200us
		}
		else 
		{
			P_DATA = 1;
			delay_us(200); //延时200us
			P_DATA = 0;
			delay_us(600); //延时600us
		}
		S_DATA = S_DATA>>1;
		B_DATA = S_DATA&0X01;
	}
	P_DATA = 1;
	delay_ms(1000); //延时5ms
}

