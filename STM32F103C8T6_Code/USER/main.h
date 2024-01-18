#ifndef __MAIN_H
#define __MAIN_H

#include "stdint.h"
//Fat weight
//	#define GPRS_BC28
//	#define Blue_APP 
	
	
typedef struct
{
//  uint8_t measure;   //���һ���Ƿ�������
//	uint8_t Height_flag;  //����Ƿ��и߶�ģ��
	uint8_t    Light_PWM_state;
	uint8_t    DeviceStatus;   // 1����    2��ͣ    3����
	
	uint16_t    Work_timer;   //ͨ��ʱ��
  
	uint16_t    Work_tim_H;     //��  ����ʹ����ʱ�� ʱ
	uint16_t    Work_tim_M;     //��  ����ʹ����ʱ�� ��
	
	uint16_t    Surplus_min;   //��  ʣ��ʹ��ʱ�� 
	uint16_t    Surplus_sec;   //��  ʣ��ʹ��ʱ�� 
	uint16_t    Set_time;      //��  ����ʹ��ʱ�� 
	
	uint16_t    Power_T;   // ��Դ�¶� 
	uint16_t    Power_FAN; //��Դ�ַ���
	uint16_t    Board_T;
	uint16_t    Light_T;   //��ͷ�¶�
	uint16_t    Light_FAN; //��ͷ����
	uint16_t    Light_PWM; //��ͷPWMֵ
} LCD_TypeDef;

extern LCD_TypeDef lcd;
extern u16 Time4_Count;  //1���ʱ��
extern u16 Work_Cnt;
//extern unsigned char Send_String[25];

extern	u16 Elight_time;	//����ʹ��ʱ��	          0     Elight_time
extern  u16 Eboard_time;	//���ư�ʹ��ʱ��	        2		  Eboard_time
extern	u16 Elight_pwm;		//����ǿ��PWMƵ��	        4     Elight_pwm
extern	u16 EBtemp_L;			//���Ӳִ��������¶ȵͣ�	6     EBtemp_L
extern	u16 EBtemp_M;			//���Ӳִ��������¶��У�	8     EBtemp_M
extern	u16 EBtemp_H;			//���Ӳִ��������¶ȸߣ�	10		EBtemp_H
 
extern	u16 ELtemp_L;			//��ѧ�ִ��������¶ȵͣ�	18    ELtemp_L
extern	u16 ELtemp_M;			//��ѧ�ִ��������¶��У�	20    ELtemp_M
extern	u16 ELtemp_H;			//��ѧ�ִ��������¶ȸߣ�	22    ELtemp_H
extern  u8 Tim_Num[50]; 	//eeprom��������

extern  u16 EBtemp_L1;			//���Ӳִ��������¶ȵͣ�	6     EBtemp_L1
extern	u16 EBtemp_M1;			//���Ӳִ��������¶��У�	8     EBtemp_M1
extern	u16 EBtemp_H1;			//���Ӳִ��������¶ȸߣ�	10		EBtemp_H1

extern	u16 ELtemp_L1;			//��ѧ�ִ��������¶ȵͣ�	18    ELtemp_L
extern	u16 ELtemp_M1;			//��ѧ�ִ��������¶��У�	20    ELtemp_M
extern	u16 ELtemp_H1;			//��ѧ�ִ��������¶ȸߣ�	22    ELtemp_H

extern unsigned char PIC_page0[10];
extern unsigned char PIC_page1[10];
extern unsigned char PIC_page5[10];
extern unsigned char PIC_page7[10]; //����

void Init_parameter(void);
void EEPROM_Data_Read(void);
void EEPROM_Data_Write(void);
#endif


