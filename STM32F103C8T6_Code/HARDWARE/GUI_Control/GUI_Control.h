#ifndef __GUI_CONTROL_H
#define __GUI_CONTROL_H
#include "sys.h"	 
#include "stdlib.h"
 
extern	u16 All_S_time;      //设置的总秒数
extern unsigned char lcd_data[8];
	
	
void util_readStm32UniqueCode(void);   //读STM32——ID号
 
void Page(char cnt);
 
void up_data(void);
 
#endif


