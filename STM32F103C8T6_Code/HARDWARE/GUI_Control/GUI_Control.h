#ifndef __GUI_CONTROL_H
#define __GUI_CONTROL_H
#include "sys.h"	 
#include "stdlib.h"
 
extern	u16 All_S_time;      //���õ�������
extern unsigned char lcd_data[8];
	
	
void util_readStm32UniqueCode(void);   //��STM32����ID��
 
void Page(char cnt);
 
void up_data(void);
 
#endif


