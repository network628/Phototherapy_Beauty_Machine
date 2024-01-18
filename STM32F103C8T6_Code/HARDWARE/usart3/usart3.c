/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*             实现串口2功能的源文件               */
/*                                                 */
/*-------------------------------------------------*/
#include "usart.h"
#include "stm32f10x.h"  //包含需要的头文件
#include "usart3.h"     //包含需要的头文件
#include "main.h"         //包含需要的头文件
#include "led.h"
#include "string.h"     //包含需要的头文件
#include "GUI_Control.h"
#include "24cxx.h" 
#include "delay.h"

#if  USART3_RX_ENABLE                   //如果使能接收功能
char Usart3_RxCompleted = 0;            //定义一个变量 0：表示接收未完成 1：表示接收完成 
unsigned int Usart3_RxCounter = 0;      //定义一个变量，记录串口2总共接收了多少字节的数据
unsigned char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //定义一个数组，用于保存串口2接收到的数据   	
#endif

/*-------------------------------------------------*/
/*函数名：初始化串口2发送功能                      */
/*参  数：bound：波特率                            */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Usart3_Init(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //定义一个设置GPIO功能的变量
	USART_InitTypeDef USART_InitStructure;   //定义一个设置串口功能的变量
#if USART3_RX_ENABLE                         //如果使能接收功能
	NVIC_InitTypeDef NVIC_InitStructure;     //如果使能接收功能，定义一个设置中断的变量
#endif

#if USART3_RX_ENABLE                                 //如果使能接收功能
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //设置中断向量分组：第2组 抢先优先级：0 1 2 3 子优先级：0 1 2 3
#endif	
      
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);  //使能串口2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //使能GPIOA时钟
	USART_DeInit(USART3);                                  //串口2寄存器重新设置为默认值
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;              //准备设置PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO速率50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //复用推挽输出，用于串口2的发送
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //设置PA2
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;              //准备设置PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入，用于串口2的接收
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //设置PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
#if USART3_RX_ENABLE               												   //如果使能接收模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //收发模式
#else                                                                              //如果不使能接收模式
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //只发模式
#endif        
    USART_Init(USART3, &USART_InitStructure);                                      //设置串口2	

#if USART3_RX_ENABLE  	         					        //如果使能接收模式
	USART_ClearFlag(USART3, USART_FLAG_RXNE);	            //清除接收标志位
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          //开启接收中断
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;       //设置串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//中断通道使能
	NVIC_Init(&NVIC_InitStructure);	                        //设置串口2中断
#endif  

	USART_Cmd(USART3, ENABLE);                              //使能串口2
}

/*-------------------------------------------------*/
/*函数名：串口2 printf函数                         */
/*参  数：char* fmt,...  格式化输出字符串和参数    */
/*返回值：无                                       */
/*-------------------------------------------------*/

__align(8) char USART3_TxBuff[USART3_TXBUFF_SIZE];  

void u3_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(USART3_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)USART3_TxBuff);		
	while((USART3->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		USART3->DR = USART3_TxBuff[i];
		while((USART3->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*函数名：串口2发送缓冲区中的数据                  */
/*参  数：data：数据                               */
/*返回值：无                                       */
/*-------------------------------------------------*/
void u3_TxData(unsigned char *data,char len)
{
	int	i;	
	while((USART3->SR&0X40)==0);
	for(i = 0;i < len;i ++){			
		USART3->DR = data[i];
		while((USART3->SR&0X40)==0);	
	}
}


 
unsigned char LCD_str[50]; 
void USART3_IRQHandler(void)   
{                      
  u8 Res;	
  u8 t,len;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
    if(Res == 0x5A) 
		{
		  Usart3_RxCounter = 0;
		}
		Usart3_RxBuff[Usart3_RxCounter]=Res ;
    Usart3_RxCounter++;
	}
 
	if(Usart3_RxBuff[0] != 0x5A && Usart3_RxBuff[1] != 0xA5) 
	{
	  Usart3_RxCounter=0;
	}
//          第0个数据为0x5A            每一次包要大于4个数      -------------------------------- 验证校验和 -------------------------------------
  if( Usart3_RxBuff[0] == 0x5A && Usart3_RxBuff[2] == 0x06 && (Usart3_RxCounter==9)) 
	{ 
 
       len = Usart3_RxCounter;
			 for(t=0;t<len;t++)  //复制字符串
			 {
				 LCD_str[t] = Usart3_RxBuff[t];
			 }	
 
		   Usart3_RxCounter=0;
			 
			 if(LCD_str[5]==0x14)  //  设定时长
			 {
			    lcd.Set_time = LCD_str[8];
				  Work_Cnt = 0;
				  lcd.DeviceStatus =1;
			 }	
			 
			 if(LCD_str[5]==0x16)  //  使能设定时长 
			 { 
			    lcd.Set_time = 5;
					lcd_data[5] = 0x14;   //设定时间
					lcd_data[6] = (lcd.Set_time>>8)&0x00ff;
					lcd_data[7] =  lcd.Set_time    &0x00ff;
					u3_TxData(lcd_data,8);
				  Work_Cnt = 0;
				  lcd.DeviceStatus =1; // 1工作    2暂停    3结束
			 }				 
			 if(LCD_str[5]==0x19 && lcd.DeviceStatus ==1 )  //  设定灯光PWM值 
			 {
			    lcd.Light_PWM = LCD_str[8];
			 }	 
			 if(LCD_str[5]==0x15)  //开始按钮
			 {
				  Work_Cnt = 0;
			    All_S_time = lcd.Set_time*60;  //设置的总秒数
				  lcd.Light_PWM = LCD_str[8]+1;
			 }	
			 if(LCD_str[5]==0x25 && LCD_str[8]==0xf3) //查看通电时长
			 {
				EEPROM_Data_Read();
				 
				lcd_data[5] = 0x17;   //分  通电时长
				lcd_data[6] = ((Eboard_time) >>8)&0x00ff;
				lcd_data[7] =  (Eboard_time) &0x00ff;
				u3_TxData(lcd_data,8);

				lcd_data[5] = 0x23;   //总时长-分 灯泡时长
				lcd_data[6] = ((Elight_time)>>8)&0x00ff;
				lcd_data[7] =  (Elight_time)    &0x00ff;
				u3_TxData(lcd_data,8);
        u1_printf("查看---Eboard_time: %d  Elight_time: %d---\r\n",Eboard_time,Elight_time);
			 }				 
       
	 
			 if(LCD_str[5]==0x47)   //进入灯泡更换设置，开始读取EEPROM灯泡使用时长到屏上显示
			 {
				lcd.Work_tim_H = Elight_time/60; //显示变量 时
				lcd_data[5] = 0x23;   //总时长-时
				lcd_data[6] = (lcd.Work_tim_H>>8)&0x00ff;
				lcd_data[7] =  lcd.Work_tim_H    &0x00ff;
				u3_TxData(lcd_data,8);
			 }
			  if(LCD_str[5]==0x45)  //复位灯泡时长
				{
				  Elight_time = 0;
					Tim_Num[0] =  Elight_time/256;    //灯泡使用时长	          0     Elight_time
					Tim_Num[1] =  Elight_time%256;
					AT24CXX_Write(0, Tim_Num,2);
				  lcd.Work_tim_H = Elight_time/60; //显示变量 时
					lcd_data[5] = 0x23;   //总时长-时
					lcd_data[6] = (lcd.Work_tim_H>>8)&0x00ff;
					lcd_data[7] =  lcd.Work_tim_H    &0x00ff;
					u3_TxData(lcd_data,8);
				}
				if(LCD_str[5]==0x46)  //复位控制板使用时长
				{
				  Eboard_time = 0;
					Tim_Num[2] = (Eboard_time>>8)&0x00ff;
					Tim_Num[3] =  Eboard_time    &0x00ff;
					AT24CXX_WriteOneByte(2,Tim_Num[2]);  //控制板使用时长	        2		  Eboard_time
					AT24CXX_WriteOneByte(3,Tim_Num[3]);
					
					lcd_data[5] = 0x17;   //总时长-时
					lcd_data[6] = (Eboard_time>>8)&0x00ff;
					lcd_data[7] =  Eboard_time    &0x00ff;
					u3_TxData(lcd_data,8);
				}
			 if(LCD_str[5]==0x48)   //进入控制板使用时长设置，开始读取EEPROM控制板使用时长到屏上显示
			 {
						u1_printf("进入控制板使用时长设置---\r\n");
			 }
			 if(LCD_str[5]==0x49)   //进入PWM频率设置，开始读取EEPROMPWM频率到屏上显示
			 {
						u1_printf("进入PWM频率设置---\r\n");
			 }
			 if(LCD_str[5]==0x51)   //进入电子仓温度设置，开始读取EEPROM电子仓温度到屏上显示
			 {
				EEPROM_Data_Read();
				 
				lcd_data[5] = 0x52;   
				lcd_data[6] = (EBtemp_L>>8)&0x00ff; //电子仓传感器 温度 最大值
				lcd_data[7] =  EBtemp_L    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x53;    
				lcd_data[6] = (EBtemp_M>>8)&0x00ff; //电子仓传感器 温度 最大值
				lcd_data[7] =  EBtemp_M    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x54;   
				lcd_data[6] = (EBtemp_H>>8)&0x00ff; //电子仓传感器 温度 最大值
				lcd_data[7] =  EBtemp_H    &0x00ff;
				u3_TxData(lcd_data,8);
				 
			  lcd_data[5] = 0x71;   
				lcd_data[6] = (EBtemp_L1>>8)&0x00ff; //电子仓传感器 温度 最小值
				lcd_data[7] =  EBtemp_L1    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x72;    
				lcd_data[6] = (EBtemp_M1>>8)&0x00ff; //电子仓传感器 温度 最小值
				lcd_data[7] =  EBtemp_M1    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x73;   
				lcd_data[6] = (EBtemp_H1>>8)&0x00ff; //电子仓传感器 温度 最小值
				lcd_data[7] =  EBtemp_H1    &0x00ff;
				u3_TxData(lcd_data,8);
				 
				lcd_data[5] = 0x55; 
				lcd_data[6] = (1000>>8)&0x00ff;
				lcd_data[7] =  1000    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x56;    
				lcd_data[6] = (2000>>8)&0x00ff;
				lcd_data[7] =  2000    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x57;   
				lcd_data[6] = (3000>>8)&0x00ff;
				lcd_data[7] =  3000    &0x00ff;
				u3_TxData(lcd_data,8);
				
				u1_printf("进入电子仓温度设置---\r\n");
				u1_printf("EBtemp_L1=%d  EBtemp_M1=%d  EBtemp_H1=%d\r\n",EBtemp_L1,EBtemp_M1,EBtemp_H1);
				u1_printf("EBtemp_L=%d  EBtemp_M=%d  EBtemp_H=%d\r\n",EBtemp_L,EBtemp_M,EBtemp_H);
				
			 }
			 if(LCD_str[5]==0x52) //低温设置写入  最大值
			 {
 		    Tim_Num[6] =	(LCD_str[7]>>8)/256;		//电子仓传感器（温度低）	6     EBtemp_L
     	  Tim_Num[7] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(6,Tim_Num[6]);
				AT24CXX_WriteOneByte(7,Tim_Num[7]);
				u1_printf("Tim_Num[6]=%d  Tim_Num[7]=%d  \r\n",Tim_Num[6],Tim_Num[7]);
			  u1_printf("EBtemp_L...\r\n");
			 }
			 if(LCD_str[5]==0x53)   //电子仓传感器（温度中）设置 最大值
			 {
				Tim_Num[8] =	(LCD_str[7]>>8)/256;		//电子仓传感器（温度低）	6     EBtemp_L
     	  Tim_Num[9] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(8,Tim_Num[8]);
				AT24CXX_WriteOneByte(9,Tim_Num[9]);
				u1_printf("Tim_Num[8]=%d  Tim_Num[9]=%d  \r\n",Tim_Num[8],Tim_Num[9]);
				u1_printf("EBtemp_M...\r\n");
			 }	
			 if(LCD_str[5]==0x54)   //电子仓传感器（温度高）设置 最大值
			 {
				Tim_Num[10] =	(LCD_str[7]>>8)/256;		//电子仓传感器（温度低）	6     EBtemp_L
     	  Tim_Num[11] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(10,Tim_Num[10]);
				AT24CXX_WriteOneByte(11,Tim_Num[11]);
				u1_printf("Tim_Num[10]=%d  Tim_Num[11]=%d  \r\n",Tim_Num[10],Tim_Num[11]);
				u1_printf("EBtemp_H...\r\n");
			 }
       if(LCD_str[5]==0x71) //低温设置写入  最小值
			 {

 		    Tim_Num[24] =	(LCD_str[7]>>8)/256;		//电子仓传感器 温度 最小值
     	  Tim_Num[25] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(24,Tim_Num[24]);
				AT24CXX_WriteOneByte(25,Tim_Num[25]);
				u1_printf("Tim_Num[24]=%d  Tim_Num[25]=%d  \r\n",Tim_Num[24],Tim_Num[25]);
			  u1_printf("EBtemp_L...\r\n");
			 }
			 if(LCD_str[5]==0x72)   //电子仓传感器（温度中）设置 最小值
			 {
				Tim_Num[26] =	(LCD_str[7]>>8)/256;		//电子仓传感器 温度 最小值
     	  Tim_Num[27] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(26,Tim_Num[26]);
				AT24CXX_WriteOneByte(27,Tim_Num[27]);
				u1_printf("Tim_Num[26]=%d  Tim_Num[27]=%d  \r\n",Tim_Num[26],Tim_Num[27]);
				u1_printf("EBtemp_M...\r\n");		 
			 }	
			 if(LCD_str[5]==0x73)   //电子仓传感器（温度高）设置 最小值
			 {
				Tim_Num[28] =	(LCD_str[7]>>8)/256;		//电子仓传感器 温度 最小值
     	  Tim_Num[29] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(28,Tim_Num[28]);
				AT24CXX_WriteOneByte(29,Tim_Num[29]);
				u1_printf("Tim_Num[28]=%d  Tim_Num[29]=%d  \r\n",Tim_Num[28],Tim_Num[29]);
				u1_printf("EBtemp_H...\r\n");	 
			 }
			 
			 if(LCD_str[5]==0x61)   //进入光学仓温度设置，开始读取EEPROM光学仓温度到屏上显示
			 {
				EEPROM_Data_Read();
	
				lcd_data[5] = 0x62;   
				lcd_data[6] = (ELtemp_L>>8)&0x00ff;  //光学仓传感器 温度 最大值
				lcd_data[7] =  ELtemp_L    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x63;    
				lcd_data[6] = (ELtemp_M>>8)&0x00ff;  //光学仓传感器 温度 最大值
				lcd_data[7] =  ELtemp_M    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x64;   
				lcd_data[6] = (ELtemp_H>>8)&0x00ff;  //光学仓传感器 温度 最大值
				lcd_data[7] =  ELtemp_H    &0x00ff;
				u3_TxData(lcd_data,8);
				
				lcd_data[5] = 0x68;   
				lcd_data[6] = (ELtemp_L1>>8)&0x00ff;  //光学仓传感器 温度 最小值
				lcd_data[7] =  ELtemp_L1    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x69;    
				lcd_data[6] = (ELtemp_M1>>8)&0x00ff;  //光学仓传感器 温度 最小值
				lcd_data[7] =  ELtemp_M1    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x70;   
				lcd_data[6] = (ELtemp_H1>>8)&0x00ff;  //光学仓传感器 温度 最小值
				lcd_data[7] =  ELtemp_H1    &0x00ff;
				u3_TxData(lcd_data,8);

				lcd_data[5] = 0x65;   
				lcd_data[6] = (1000>>8)&0x00ff;
				lcd_data[7] =  1000    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x66;    
				lcd_data[6] = (2000>>8)&0x00ff;
				lcd_data[7] =  2000    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x67;   
				lcd_data[6] = (3000>>8)&0x00ff;
				lcd_data[7] =  3000    &0x00ff;
				u3_TxData(lcd_data,8);
				
				u1_printf("进入光学仓温度设置---\r\n");
				u1_printf("ELtemp_L1=%d  ELtemp_M1=%d  ELtemp_H1=%d\r\n",ELtemp_L1,ELtemp_M1,ELtemp_H1);
				u1_printf("ELtemp_L=%d  ELtemp_M=%d  ELtemp_H=%d\r\n",ELtemp_L,ELtemp_M,ELtemp_H);
			 }
			 if(LCD_str[5]==0x62)   //光学仓传感器（温度低）设置
			 {
				Tim_Num[18] =	(LCD_str[7]>>8)/256;		//光学仓传感器 温度 最大值
     	  Tim_Num[19] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(18,Tim_Num[18]);
				AT24CXX_WriteOneByte(19,Tim_Num[19]);
				u1_printf("Tim_Num[18=%d  Tim_Num[19]=%d  \r\n",Tim_Num[18],Tim_Num[19]);
				u1_printf("ELtemp_L...\r\n");
			 }
			 if(LCD_str[5]==0x63)   //光学仓传感器（温度中）设置
			 {
				Tim_Num[20] =	(LCD_str[7]>>8)/256;		//光学仓传感器 温度 最大值
     	  Tim_Num[21] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(20,Tim_Num[20]);
				AT24CXX_WriteOneByte(21,Tim_Num[21]);
				u1_printf("Tim_Num[20=%d  Tim_Num[21]=%d  \r\n",Tim_Num[20],Tim_Num[21]);
			  u1_printf("ELtemp_M...\r\n");
			 }	
			 if(LCD_str[5]==0x64)   //光学仓传感器（温度高）设置
			 {
				Tim_Num[22] =	(LCD_str[7]>>8)/256;		//光学仓传感器 温度 最大值
     	  Tim_Num[23] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(22,Tim_Num[22]);
				AT24CXX_WriteOneByte(23,Tim_Num[23]);
				u1_printf("Tim_Num[22=%d  Tim_Num[23]=%d  \r\n",Tim_Num[22],Tim_Num[23]);
				u1_printf("ELtemp_H...\r\n");
			 }
			 
			 if(LCD_str[5]==0x68)   //光学仓传感器（温度低）设置
			 {
				Tim_Num[30] =	(LCD_str[7]>>8)/256;		//光学仓传感器 温度 最小值
     	  Tim_Num[31] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(30,Tim_Num[30]);
				AT24CXX_WriteOneByte(31,Tim_Num[31]);
				u1_printf("Tim_Num[30]=%d  Tim_Num[31]=%d  \r\n",Tim_Num[30],Tim_Num[31]);
				u1_printf("ELtemp_L...\r\n");
			 }
			 if(LCD_str[5]==0x69)   //光学仓传感器（温度中）设置
			 {
				Tim_Num[32] =	(LCD_str[7]>>8)/256;		//光学仓传感器 温度 最小值
     	  Tim_Num[33] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(32,Tim_Num[32]);
				AT24CXX_WriteOneByte(33,Tim_Num[33]);
				u1_printf("Tim_Num[32]=%d  Tim_Num[33]=%d  \r\n",Tim_Num[32],Tim_Num[33]);
			  u1_printf("ELtemp_M...\r\n");
			 }	
			 if(LCD_str[5]==0x70)   //光学仓传感器（温度高）设置
			 {
				Tim_Num[34] =	(LCD_str[7]>>8)/256;		//光学仓传感器 温度 最小值
     	  Tim_Num[35] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(34,Tim_Num[34]);
				AT24CXX_WriteOneByte(35,Tim_Num[35]);
				u1_printf("Tim_Num[34]=%d  Tim_Num[35]=%d  \r\n",Tim_Num[34],Tim_Num[35]);
				u1_printf("ELtemp_H...\r\n");
			 }
			 
       if(LCD_str[5]==0x50)   //设置光照强度PWM占空比
			 {
					Elight_pwm  = LCD_str[8];
					Tim_Num[4] =	(LCD_str[7]>>8)/256;		//光照强度PWM占空比
					Tim_Num[5] =  LCD_str[8]    %256;
					AT24CXX_WriteOneByte(4,Tim_Num[4]);
					AT24CXX_WriteOneByte(5,Tim_Num[5]);
					u1_printf("PWM_SET=%d...\r\n",Elight_pwm);
			 }	
			 
       if(LCD_str[5]==0x25 && LCD_str[8]==0xf4) //进入系统设置
			 {
			    EEPROM_Data_Read();
					lcd_data[5] = 0x50;   //分  通电时长  //光照强度PWM占空比显示
					lcd_data[6] = ((Elight_pwm) >>8)&0x00ff;
					lcd_data[7] =  (Elight_pwm) &0x00ff;
					u3_TxData(lcd_data,8);
       }
	 }	
}
 
  


