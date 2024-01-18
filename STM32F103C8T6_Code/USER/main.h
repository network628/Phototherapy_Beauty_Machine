#ifndef __MAIN_H
#define __MAIN_H

#include "stdint.h"
//Fat weight
//	#define GPRS_BC28
//	#define Blue_APP 
	
	
typedef struct
{
//  uint8_t measure;   //检测一次是否测量完成
//	uint8_t Height_flag;  //检测是否有高度模块
	uint8_t    Light_PWM_state;
	uint8_t    DeviceStatus;   // 1工作    2暂停    3结束
	
	uint16_t    Work_timer;   //通电时长
  
	uint16_t    Work_tim_H;     //分  工作使用总时间 时
	uint16_t    Work_tim_M;     //分  工作使用总时间 分
	
	uint16_t    Surplus_min;   //分  剩余使用时间 
	uint16_t    Surplus_sec;   //秒  剩余使用时间 
	uint16_t    Set_time;      //分  设置使用时间 
	
	uint16_t    Power_T;   // 电源温度 
	uint16_t    Power_FAN; //电源仓风扇
	uint16_t    Board_T;
	uint16_t    Light_T;   //灯头温度
	uint16_t    Light_FAN; //灯头风扇
	uint16_t    Light_PWM; //灯头PWM值
} LCD_TypeDef;

extern LCD_TypeDef lcd;
extern u16 Time4_Count;  //1秒计时器
extern u16 Work_Cnt;
//extern unsigned char Send_String[25];

extern	u16 Elight_time;	//灯泡使用时长	          0     Elight_time
extern  u16 Eboard_time;	//控制板使用时长	        2		  Eboard_time
extern	u16 Elight_pwm;		//光照强度PWM频率	        4     Elight_pwm
extern	u16 EBtemp_L;			//电子仓传感器（温度低）	6     EBtemp_L
extern	u16 EBtemp_M;			//电子仓传感器（温度中）	8     EBtemp_M
extern	u16 EBtemp_H;			//电子仓传感器（温度高）	10		EBtemp_H
 
extern	u16 ELtemp_L;			//光学仓传感器（温度低）	18    ELtemp_L
extern	u16 ELtemp_M;			//光学仓传感器（温度中）	20    ELtemp_M
extern	u16 ELtemp_H;			//光学仓传感器（温度高）	22    ELtemp_H
extern  u8 Tim_Num[50]; 	//eeprom缓存数组

extern  u16 EBtemp_L1;			//电子仓传感器（温度低）	6     EBtemp_L1
extern	u16 EBtemp_M1;			//电子仓传感器（温度中）	8     EBtemp_M1
extern	u16 EBtemp_H1;			//电子仓传感器（温度高）	10		EBtemp_H1

extern	u16 ELtemp_L1;			//光学仓传感器（温度低）	18    ELtemp_L
extern	u16 ELtemp_M1;			//光学仓传感器（温度中）	20    ELtemp_M
extern	u16 ELtemp_H1;			//光学仓传感器（温度高）	22    ELtemp_H

extern unsigned char PIC_page0[10];
extern unsigned char PIC_page1[10];
extern unsigned char PIC_page5[10];
extern unsigned char PIC_page7[10]; //结束

void Init_parameter(void);
void EEPROM_Data_Read(void);
void EEPROM_Data_Write(void);
#endif


