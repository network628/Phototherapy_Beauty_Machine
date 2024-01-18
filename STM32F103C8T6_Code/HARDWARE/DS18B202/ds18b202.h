#ifndef __DS18B202_H
#define __DS18B202_H 
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
#define DS18B20_IO_IN2()  {GPIOC->CRH&=0XFFFFFFF0;GPIOC->CRH|=0X00000008;}
#define DS18B20_IO_OUT2() {GPIOC->CRH&=0XFFFFFFF0;GPIOC->CRH|=0X00000003;}
////IO��������											   
#define	DS18B20_DQ_OUT2 PCout(8) //���ݶ˿�	PA0
#define	DS18B20_DQ_IN2  PCin(8)  //���ݶ˿�	PA0 
   	
u8 DS18B20_Init2(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp2(void);	//��ȡ�¶�
void DS18B20_Start2(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte2(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte2(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit2(void);		//����һ��λ
u8 DS18B20_Check2(void);			//����Ƿ����DS18B20
void DS18B20_Rst2(void);			//��λDS18B20    
#endif















