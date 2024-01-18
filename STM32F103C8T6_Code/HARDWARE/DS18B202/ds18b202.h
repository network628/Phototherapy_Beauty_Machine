#ifndef __DS18B202_H
#define __DS18B202_H 
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
#define DS18B20_IO_IN2()  {GPIOC->CRH&=0XFFFFFFF0;GPIOC->CRH|=0X00000008;}
#define DS18B20_IO_OUT2() {GPIOC->CRH&=0XFFFFFFF0;GPIOC->CRH|=0X00000003;}
////IO操作函数											   
#define	DS18B20_DQ_OUT2 PCout(8) //数据端口	PA0
#define	DS18B20_DQ_IN2  PCin(8)  //数据端口	PA0 
   	
u8 DS18B20_Init2(void);			//初始化DS18B20
short DS18B20_Get_Temp2(void);	//获取温度
void DS18B20_Start2(void);		//开始温度转换
void DS18B20_Write_Byte2(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte2(void);		//读出一个字节
u8 DS18B20_Read_Bit2(void);		//读出一个位
u8 DS18B20_Check2(void);			//检测是否存在DS18B20
void DS18B20_Rst2(void);			//复位DS18B20    
#endif















