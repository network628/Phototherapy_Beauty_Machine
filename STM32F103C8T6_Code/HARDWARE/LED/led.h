#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define BEEP PAout(1)	// BEEP
#define LED1 PCout(12)	// LED
#define P_DATA PCout(11) //DATA

#define WTN6_BUSY  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)  //BUSY

void LED_Init(void);//��ʼ��
void Line_1A_WTN6(u8 DDATA);
		 				    
#endif
