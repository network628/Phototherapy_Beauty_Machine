#ifndef __DS18B203_H
#define __DS18B203_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniSTM32开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//IO方向设置
#define DS18B20_IO_IN3()  {GPIOC->CRH&=0XFFFFFF0F;GPIOC->CRH|=0X00000080;}
#define DS18B20_IO_OUT3() {GPIOC->CRH&=0XFFFFFF0F;GPIOC->CRH|=0X00000030;}
////IO操作函数											   
#define	DS18B20_DQ_OUT3 PCout(9) //数据端口	PA0
#define	DS18B20_DQ_IN3  PCin(9)  //数据端口	PA0 
   	
u8 DS18B20_Init3(void);			//初始化DS18B20
short DS18B20_Get_Temp3(void);	//获取温度
void DS18B20_Start3(void);		//开始温度转换
void DS18B20_Write_Byte3(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte3(void);		//读出一个字节
u8 DS18B20_Read_Bit3(void);		//读出一个位
u8 DS18B20_Check3(void);			//检测是否存在DS18B20
void DS18B20_Rst3(void);			//复位DS18B20    
#endif















