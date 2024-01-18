#ifndef __DS18B201_H
#define __DS18B201_H 
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
#define DS18B20_IO_IN()  {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=0X80000000;}
#define DS18B20_IO_OUT() {GPIOC->CRL&=0X0FFFFFFF;GPIOC->CRL|=0X30000000;}
////IO��������											   
#define	DS18B20_DQ_OUT PCout(7) //���ݶ˿�	PA0
#define	DS18B20_DQ_IN  PCin(7)  //���ݶ˿�	PA0 
   	
u8 DS18B20_Init1(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp1(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    
#endif















