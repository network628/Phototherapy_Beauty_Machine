#include "ds18b203.h"
#include "delay.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
  
#define ds_s  0
//��λDS18B20
void DS18B20_Rst3(void)	   
{                 
	DS18B20_IO_OUT3(); //SET PA0 OUTPUT
    DS18B20_DQ_OUT3=0; //����DQ
    delay_us(750+ds_s);    //����750us
    DS18B20_DQ_OUT3=1; //DQ=1 
	delay_us(15+ds_s);     //15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
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
//��DS18B20��ȡһ��λ
//����ֵ��1/0
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
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
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
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
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
//��ʼ�¶�ת��
void DS18B20_Start3(void)// ds1820 start convert
{   						               
    DS18B20_Rst3();	   
	DS18B20_Check3();	 
    DS18B20_Write_Byte3(0xcc);// skip rom
    DS18B20_Write_Byte3(0x44);// convert
} 
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init3(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PORTA��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//PORTA0 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOC,GPIO_Pin_9);    //���1

	DS18B20_Rst3();

	return DS18B20_Check3();
}  
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
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
        temp=0;//�¶�Ϊ��  
    }else temp=1;//�¶�Ϊ��	  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(float)tem*0.625;//ת��     
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
} 
 
