#include "ds18b203.h"
#include "delay.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
  
#define ds_s  0
//复位DS18B20
void DS18B20_Rst3(void)	   
{                 
	DS18B20_IO_OUT3(); //SET PA0 OUTPUT
    DS18B20_DQ_OUT3=0; //拉低DQ
    delay_us(750+ds_s);    //拉低750us
    DS18B20_DQ_OUT3=1; //DQ=1 
	delay_us(15+ds_s);     //15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
u8 DS18B20_Check3(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN3();//SET PA0 INPUT	 
    while (DS18B20_DQ_IN3&&retry<200)
	{
		retry++;
		delay_us(1+ds_s);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN3&&retry<240)
	{
		retry++;
		delay_us(1+ds_s);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
u8 DS18B20_Read_Bit3(void) 			 // read one bit
{
    u8 data;
	DS18B20_IO_OUT3();//SET PA0 OUTPUT
    DS18B20_DQ_OUT3=0; 
	delay_us(2+ds_s);
    DS18B20_DQ_OUT3=1; 
	DS18B20_IO_IN3();//SET PA0 INPUT
	delay_us(12+ds_s);
	if(DS18B20_DQ_IN3)data=1;
    else data=0;	 
    delay_us(50+ds_s);           
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
u8 DS18B20_Read_Byte3(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit3();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte3(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT3();//SET PA0 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT3=0;// Write 1
            delay_us(2+ds_s);                            
            DS18B20_DQ_OUT3=1;
            delay_us(60+ds_s);             
        }
        else 
        {
            DS18B20_DQ_OUT3=0;// Write 0
            delay_us(60+ds_s);             
            DS18B20_DQ_OUT3=1;
            delay_us(2+ds_s);                          
        }
    }
}
//开始温度转换
void DS18B20_Start3(void)// ds1820 start convert
{   						               
    DS18B20_Rst3();	   
	DS18B20_Check3();	 
    DS18B20_Write_Byte3(0xcc);// skip rom
    DS18B20_Write_Byte3(0x44);// convert
} 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS18B20_Init3(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PORTA口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//PORTA0 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOC,GPIO_Pin_9);    //输出1

	DS18B20_Rst3();

	return DS18B20_Check3();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp3(void)
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Start3 ();                    // ds1820 start convert
    DS18B20_Rst3();
    DS18B20_Check3();	 
    DS18B20_Write_Byte3(0xcc);// skip rom
    DS18B20_Write_Byte3(0xbe);// convert	    
    TL=DS18B20_Read_Byte3(); // LSB   
    TH=DS18B20_Read_Byte3(); // MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(float)tem*0.625;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
} 
 
