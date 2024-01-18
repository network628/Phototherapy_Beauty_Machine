#include "led.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK mini�SSTM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PD�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //BEEP
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PA.8 �����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;	    		 //TG1  LED 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	GPIO_ResetBits(GPIOC,GPIO_Pin_11 | GPIO_Pin_12); 						 //PD.2 ����� 

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;//PC10  BUSY
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ�� 
}
 
 

/*--------------------------------------
;ģ������:Line_1A_WTN6(UC8 DDATA)
;����:ʵ��һ�ߴ���ͨ�ź���
;���: DDATA Ϊ��������
;����:
;P_DATA Ϊ���ݿ�
;-------------------------------------*/
void Line_1A_WTN6(u8 DDATA)
{
	u8 S_DATA,j;
	char B_DATA;
	S_DATA = DDATA;
	P_DATA = 0;
	delay_ms(500); //��ʱ5ms
	B_DATA = S_DATA&0X01;
	for(j=0;j<8;j++)
	{
		if(B_DATA == 1)
		{
			P_DATA = 1;
			delay_us(600); //��ʱ600us
			
			P_DATA = 0;
			delay_us(200); //��ʱ200us
		}
		else 
		{
			P_DATA = 1;
			delay_us(200); //��ʱ200us
			P_DATA = 0;
			delay_us(600); //��ʱ600us
		}
		S_DATA = S_DATA>>1;
		B_DATA = S_DATA&0X01;
	}
	P_DATA = 1;
	delay_ms(1000); //��ʱ5ms
}

