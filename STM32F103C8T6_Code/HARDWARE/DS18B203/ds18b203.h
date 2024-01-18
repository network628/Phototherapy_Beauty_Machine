#ifndef __DS18B203_H
#define __DS18B203_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniSTM32������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//IO��������
#define DS18B20_IO_IN3()  {GPIOC->CRH&=0XFFFFFF0F;GPIOC->CRH|=0X00000080;}
#define DS18B20_IO_OUT3() {GPIOC->CRH&=0XFFFFFF0F;GPIOC->CRH|=0X00000030;}
////IO��������											   
#define	DS18B20_DQ_OUT3 PCout(9) //���ݶ˿�	PA0
#define	DS18B20_DQ_IN3  PCin(9)  //���ݶ˿�	PA0 
   	
u8 DS18B20_Init3(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp3(void);	//��ȡ�¶�
void DS18B20_Start3(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte3(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte3(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit3(void);		//����һ��λ
u8 DS18B20_Check3(void);			//����Ƿ����DS18B20
void DS18B20_Rst3(void);			//��λDS18B20    
#endif















