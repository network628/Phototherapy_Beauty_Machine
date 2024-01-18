/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ�ִ���2���ܵ�Դ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/
#include "usart.h"
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "usart3.h"     //������Ҫ��ͷ�ļ�
#include "main.h"         //������Ҫ��ͷ�ļ�
#include "led.h"
#include "string.h"     //������Ҫ��ͷ�ļ�
#include "GUI_Control.h"
#include "24cxx.h" 
#include "delay.h"

#if  USART3_RX_ENABLE                   //���ʹ�ܽ��չ���
char Usart3_RxCompleted = 0;            //����һ������ 0����ʾ����δ��� 1����ʾ������� 
unsigned int Usart3_RxCounter = 0;      //����һ����������¼����2�ܹ������˶����ֽڵ�����
unsigned char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //����һ�����飬���ڱ��洮��2���յ�������   	
#endif

/*-------------------------------------------------*/
/*����������ʼ������2���͹���                      */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Usart3_Init(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //����һ������GPIO���ܵı���
	USART_InitTypeDef USART_InitStructure;   //����һ�����ô��ڹ��ܵı���
#if USART3_RX_ENABLE                         //���ʹ�ܽ��չ���
	NVIC_InitTypeDef NVIC_InitStructure;     //���ʹ�ܽ��չ��ܣ�����һ�������жϵı���
#endif

#if USART3_RX_ENABLE                                 //���ʹ�ܽ��չ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�����ж��������飺��2�� �������ȼ���0 1 2 3 �����ȼ���0 1 2 3
#endif	
      
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);  //ʹ�ܴ���2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   //ʹ��GPIOAʱ��
	USART_DeInit(USART3);                                  //����2�Ĵ�����������ΪĬ��ֵ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;              //׼������PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO����50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //����������������ڴ���2�ķ���
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //����PA2
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;              //׼������PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�������룬���ڴ���2�Ľ���
    GPIO_Init(GPIOB, &GPIO_InitStructure);                 //����PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
#if USART3_RX_ENABLE               												   //���ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //�շ�ģʽ
#else                                                                              //�����ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //ֻ��ģʽ
#endif        
    USART_Init(USART3, &USART_InitStructure);                                      //���ô���2	

#if USART3_RX_ENABLE  	         					        //���ʹ�ܽ���ģʽ
	USART_ClearFlag(USART3, USART_FLAG_RXNE);	            //������ձ�־λ
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);          //���������ж�
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;       //���ô���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //���ô���2�ж�
#endif  

	USART_Cmd(USART3, ENABLE);                              //ʹ�ܴ���2
}

/*-------------------------------------------------*/
/*������������2 printf����                         */
/*��  ����char* fmt,...  ��ʽ������ַ����Ͳ���    */
/*����ֵ����                                       */
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
/*������������2���ͻ������е�����                  */
/*��  ����data������                               */
/*����ֵ����                                       */
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
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
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
//          ��0������Ϊ0x5A            ÿһ�ΰ�Ҫ����4����      -------------------------------- ��֤У��� -------------------------------------
  if( Usart3_RxBuff[0] == 0x5A && Usart3_RxBuff[2] == 0x06 && (Usart3_RxCounter==9)) 
	{ 
 
       len = Usart3_RxCounter;
			 for(t=0;t<len;t++)  //�����ַ���
			 {
				 LCD_str[t] = Usart3_RxBuff[t];
			 }	
 
		   Usart3_RxCounter=0;
			 
			 if(LCD_str[5]==0x14)  //  �趨ʱ��
			 {
			    lcd.Set_time = LCD_str[8];
				  Work_Cnt = 0;
				  lcd.DeviceStatus =1;
			 }	
			 
			 if(LCD_str[5]==0x16)  //  ʹ���趨ʱ�� 
			 { 
			    lcd.Set_time = 5;
					lcd_data[5] = 0x14;   //�趨ʱ��
					lcd_data[6] = (lcd.Set_time>>8)&0x00ff;
					lcd_data[7] =  lcd.Set_time    &0x00ff;
					u3_TxData(lcd_data,8);
				  Work_Cnt = 0;
				  lcd.DeviceStatus =1; // 1����    2��ͣ    3����
			 }				 
			 if(LCD_str[5]==0x19 && lcd.DeviceStatus ==1 )  //  �趨�ƹ�PWMֵ 
			 {
			    lcd.Light_PWM = LCD_str[8];
			 }	 
			 if(LCD_str[5]==0x15)  //��ʼ��ť
			 {
				  Work_Cnt = 0;
			    All_S_time = lcd.Set_time*60;  //���õ�������
				  lcd.Light_PWM = LCD_str[8]+1;
			 }	
			 if(LCD_str[5]==0x25 && LCD_str[8]==0xf3) //�鿴ͨ��ʱ��
			 {
				EEPROM_Data_Read();
				 
				lcd_data[5] = 0x17;   //��  ͨ��ʱ��
				lcd_data[6] = ((Eboard_time) >>8)&0x00ff;
				lcd_data[7] =  (Eboard_time) &0x00ff;
				u3_TxData(lcd_data,8);

				lcd_data[5] = 0x23;   //��ʱ��-�� ����ʱ��
				lcd_data[6] = ((Elight_time)>>8)&0x00ff;
				lcd_data[7] =  (Elight_time)    &0x00ff;
				u3_TxData(lcd_data,8);
        u1_printf("�鿴---Eboard_time: %d  Elight_time: %d---\r\n",Eboard_time,Elight_time);
			 }				 
       
	 
			 if(LCD_str[5]==0x47)   //������ݸ������ã���ʼ��ȡEEPROM����ʹ��ʱ����������ʾ
			 {
				lcd.Work_tim_H = Elight_time/60; //��ʾ���� ʱ
				lcd_data[5] = 0x23;   //��ʱ��-ʱ
				lcd_data[6] = (lcd.Work_tim_H>>8)&0x00ff;
				lcd_data[7] =  lcd.Work_tim_H    &0x00ff;
				u3_TxData(lcd_data,8);
			 }
			  if(LCD_str[5]==0x45)  //��λ����ʱ��
				{
				  Elight_time = 0;
					Tim_Num[0] =  Elight_time/256;    //����ʹ��ʱ��	          0     Elight_time
					Tim_Num[1] =  Elight_time%256;
					AT24CXX_Write(0, Tim_Num,2);
				  lcd.Work_tim_H = Elight_time/60; //��ʾ���� ʱ
					lcd_data[5] = 0x23;   //��ʱ��-ʱ
					lcd_data[6] = (lcd.Work_tim_H>>8)&0x00ff;
					lcd_data[7] =  lcd.Work_tim_H    &0x00ff;
					u3_TxData(lcd_data,8);
				}
				if(LCD_str[5]==0x46)  //��λ���ư�ʹ��ʱ��
				{
				  Eboard_time = 0;
					Tim_Num[2] = (Eboard_time>>8)&0x00ff;
					Tim_Num[3] =  Eboard_time    &0x00ff;
					AT24CXX_WriteOneByte(2,Tim_Num[2]);  //���ư�ʹ��ʱ��	        2		  Eboard_time
					AT24CXX_WriteOneByte(3,Tim_Num[3]);
					
					lcd_data[5] = 0x17;   //��ʱ��-ʱ
					lcd_data[6] = (Eboard_time>>8)&0x00ff;
					lcd_data[7] =  Eboard_time    &0x00ff;
					u3_TxData(lcd_data,8);
				}
			 if(LCD_str[5]==0x48)   //������ư�ʹ��ʱ�����ã���ʼ��ȡEEPROM���ư�ʹ��ʱ����������ʾ
			 {
						u1_printf("������ư�ʹ��ʱ������---\r\n");
			 }
			 if(LCD_str[5]==0x49)   //����PWMƵ�����ã���ʼ��ȡEEPROMPWMƵ�ʵ�������ʾ
			 {
						u1_printf("����PWMƵ������---\r\n");
			 }
			 if(LCD_str[5]==0x51)   //������Ӳ��¶����ã���ʼ��ȡEEPROM���Ӳ��¶ȵ�������ʾ
			 {
				EEPROM_Data_Read();
				 
				lcd_data[5] = 0x52;   
				lcd_data[6] = (EBtemp_L>>8)&0x00ff; //���Ӳִ����� �¶� ���ֵ
				lcd_data[7] =  EBtemp_L    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x53;    
				lcd_data[6] = (EBtemp_M>>8)&0x00ff; //���Ӳִ����� �¶� ���ֵ
				lcd_data[7] =  EBtemp_M    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x54;   
				lcd_data[6] = (EBtemp_H>>8)&0x00ff; //���Ӳִ����� �¶� ���ֵ
				lcd_data[7] =  EBtemp_H    &0x00ff;
				u3_TxData(lcd_data,8);
				 
			  lcd_data[5] = 0x71;   
				lcd_data[6] = (EBtemp_L1>>8)&0x00ff; //���Ӳִ����� �¶� ��Сֵ
				lcd_data[7] =  EBtemp_L1    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x72;    
				lcd_data[6] = (EBtemp_M1>>8)&0x00ff; //���Ӳִ����� �¶� ��Сֵ
				lcd_data[7] =  EBtemp_M1    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x73;   
				lcd_data[6] = (EBtemp_H1>>8)&0x00ff; //���Ӳִ����� �¶� ��Сֵ
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
				
				u1_printf("������Ӳ��¶�����---\r\n");
				u1_printf("EBtemp_L1=%d  EBtemp_M1=%d  EBtemp_H1=%d\r\n",EBtemp_L1,EBtemp_M1,EBtemp_H1);
				u1_printf("EBtemp_L=%d  EBtemp_M=%d  EBtemp_H=%d\r\n",EBtemp_L,EBtemp_M,EBtemp_H);
				
			 }
			 if(LCD_str[5]==0x52) //��������д��  ���ֵ
			 {
 		    Tim_Num[6] =	(LCD_str[7]>>8)/256;		//���Ӳִ��������¶ȵͣ�	6     EBtemp_L
     	  Tim_Num[7] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(6,Tim_Num[6]);
				AT24CXX_WriteOneByte(7,Tim_Num[7]);
				u1_printf("Tim_Num[6]=%d  Tim_Num[7]=%d  \r\n",Tim_Num[6],Tim_Num[7]);
			  u1_printf("EBtemp_L...\r\n");
			 }
			 if(LCD_str[5]==0x53)   //���Ӳִ��������¶��У����� ���ֵ
			 {
				Tim_Num[8] =	(LCD_str[7]>>8)/256;		//���Ӳִ��������¶ȵͣ�	6     EBtemp_L
     	  Tim_Num[9] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(8,Tim_Num[8]);
				AT24CXX_WriteOneByte(9,Tim_Num[9]);
				u1_printf("Tim_Num[8]=%d  Tim_Num[9]=%d  \r\n",Tim_Num[8],Tim_Num[9]);
				u1_printf("EBtemp_M...\r\n");
			 }	
			 if(LCD_str[5]==0x54)   //���Ӳִ��������¶ȸߣ����� ���ֵ
			 {
				Tim_Num[10] =	(LCD_str[7]>>8)/256;		//���Ӳִ��������¶ȵͣ�	6     EBtemp_L
     	  Tim_Num[11] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(10,Tim_Num[10]);
				AT24CXX_WriteOneByte(11,Tim_Num[11]);
				u1_printf("Tim_Num[10]=%d  Tim_Num[11]=%d  \r\n",Tim_Num[10],Tim_Num[11]);
				u1_printf("EBtemp_H...\r\n");
			 }
       if(LCD_str[5]==0x71) //��������д��  ��Сֵ
			 {

 		    Tim_Num[24] =	(LCD_str[7]>>8)/256;		//���Ӳִ����� �¶� ��Сֵ
     	  Tim_Num[25] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(24,Tim_Num[24]);
				AT24CXX_WriteOneByte(25,Tim_Num[25]);
				u1_printf("Tim_Num[24]=%d  Tim_Num[25]=%d  \r\n",Tim_Num[24],Tim_Num[25]);
			  u1_printf("EBtemp_L...\r\n");
			 }
			 if(LCD_str[5]==0x72)   //���Ӳִ��������¶��У����� ��Сֵ
			 {
				Tim_Num[26] =	(LCD_str[7]>>8)/256;		//���Ӳִ����� �¶� ��Сֵ
     	  Tim_Num[27] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(26,Tim_Num[26]);
				AT24CXX_WriteOneByte(27,Tim_Num[27]);
				u1_printf("Tim_Num[26]=%d  Tim_Num[27]=%d  \r\n",Tim_Num[26],Tim_Num[27]);
				u1_printf("EBtemp_M...\r\n");		 
			 }	
			 if(LCD_str[5]==0x73)   //���Ӳִ��������¶ȸߣ����� ��Сֵ
			 {
				Tim_Num[28] =	(LCD_str[7]>>8)/256;		//���Ӳִ����� �¶� ��Сֵ
     	  Tim_Num[29] =   LCD_str[8]    %256;
				AT24CXX_WriteOneByte(28,Tim_Num[28]);
				AT24CXX_WriteOneByte(29,Tim_Num[29]);
				u1_printf("Tim_Num[28]=%d  Tim_Num[29]=%d  \r\n",Tim_Num[28],Tim_Num[29]);
				u1_printf("EBtemp_H...\r\n");	 
			 }
			 
			 if(LCD_str[5]==0x61)   //�����ѧ���¶����ã���ʼ��ȡEEPROM��ѧ���¶ȵ�������ʾ
			 {
				EEPROM_Data_Read();
	
				lcd_data[5] = 0x62;   
				lcd_data[6] = (ELtemp_L>>8)&0x00ff;  //��ѧ�ִ����� �¶� ���ֵ
				lcd_data[7] =  ELtemp_L    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x63;    
				lcd_data[6] = (ELtemp_M>>8)&0x00ff;  //��ѧ�ִ����� �¶� ���ֵ
				lcd_data[7] =  ELtemp_M    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x64;   
				lcd_data[6] = (ELtemp_H>>8)&0x00ff;  //��ѧ�ִ����� �¶� ���ֵ
				lcd_data[7] =  ELtemp_H    &0x00ff;
				u3_TxData(lcd_data,8);
				
				lcd_data[5] = 0x68;   
				lcd_data[6] = (ELtemp_L1>>8)&0x00ff;  //��ѧ�ִ����� �¶� ��Сֵ
				lcd_data[7] =  ELtemp_L1    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x69;    
				lcd_data[6] = (ELtemp_M1>>8)&0x00ff;  //��ѧ�ִ����� �¶� ��Сֵ
				lcd_data[7] =  ELtemp_M1    &0x00ff;
				u3_TxData(lcd_data,8);
				lcd_data[5] = 0x70;   
				lcd_data[6] = (ELtemp_H1>>8)&0x00ff;  //��ѧ�ִ����� �¶� ��Сֵ
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
				
				u1_printf("�����ѧ���¶�����---\r\n");
				u1_printf("ELtemp_L1=%d  ELtemp_M1=%d  ELtemp_H1=%d\r\n",ELtemp_L1,ELtemp_M1,ELtemp_H1);
				u1_printf("ELtemp_L=%d  ELtemp_M=%d  ELtemp_H=%d\r\n",ELtemp_L,ELtemp_M,ELtemp_H);
			 }
			 if(LCD_str[5]==0x62)   //��ѧ�ִ��������¶ȵͣ�����
			 {
				Tim_Num[18] =	(LCD_str[7]>>8)/256;		//��ѧ�ִ����� �¶� ���ֵ
     	  Tim_Num[19] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(18,Tim_Num[18]);
				AT24CXX_WriteOneByte(19,Tim_Num[19]);
				u1_printf("Tim_Num[18=%d  Tim_Num[19]=%d  \r\n",Tim_Num[18],Tim_Num[19]);
				u1_printf("ELtemp_L...\r\n");
			 }
			 if(LCD_str[5]==0x63)   //��ѧ�ִ��������¶��У�����
			 {
				Tim_Num[20] =	(LCD_str[7]>>8)/256;		//��ѧ�ִ����� �¶� ���ֵ
     	  Tim_Num[21] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(20,Tim_Num[20]);
				AT24CXX_WriteOneByte(21,Tim_Num[21]);
				u1_printf("Tim_Num[20=%d  Tim_Num[21]=%d  \r\n",Tim_Num[20],Tim_Num[21]);
			  u1_printf("ELtemp_M...\r\n");
			 }	
			 if(LCD_str[5]==0x64)   //��ѧ�ִ��������¶ȸߣ�����
			 {
				Tim_Num[22] =	(LCD_str[7]>>8)/256;		//��ѧ�ִ����� �¶� ���ֵ
     	  Tim_Num[23] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(22,Tim_Num[22]);
				AT24CXX_WriteOneByte(23,Tim_Num[23]);
				u1_printf("Tim_Num[22=%d  Tim_Num[23]=%d  \r\n",Tim_Num[22],Tim_Num[23]);
				u1_printf("ELtemp_H...\r\n");
			 }
			 
			 if(LCD_str[5]==0x68)   //��ѧ�ִ��������¶ȵͣ�����
			 {
				Tim_Num[30] =	(LCD_str[7]>>8)/256;		//��ѧ�ִ����� �¶� ��Сֵ
     	  Tim_Num[31] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(30,Tim_Num[30]);
				AT24CXX_WriteOneByte(31,Tim_Num[31]);
				u1_printf("Tim_Num[30]=%d  Tim_Num[31]=%d  \r\n",Tim_Num[30],Tim_Num[31]);
				u1_printf("ELtemp_L...\r\n");
			 }
			 if(LCD_str[5]==0x69)   //��ѧ�ִ��������¶��У�����
			 {
				Tim_Num[32] =	(LCD_str[7]>>8)/256;		//��ѧ�ִ����� �¶� ��Сֵ
     	  Tim_Num[33] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(32,Tim_Num[32]);
				AT24CXX_WriteOneByte(33,Tim_Num[33]);
				u1_printf("Tim_Num[32]=%d  Tim_Num[33]=%d  \r\n",Tim_Num[32],Tim_Num[33]);
			  u1_printf("ELtemp_M...\r\n");
			 }	
			 if(LCD_str[5]==0x70)   //��ѧ�ִ��������¶ȸߣ�����
			 {
				Tim_Num[34] =	(LCD_str[7]>>8)/256;		//��ѧ�ִ����� �¶� ��Сֵ
     	  Tim_Num[35] =  LCD_str[8]    %256;
				AT24CXX_WriteOneByte(34,Tim_Num[34]);
				AT24CXX_WriteOneByte(35,Tim_Num[35]);
				u1_printf("Tim_Num[34]=%d  Tim_Num[35]=%d  \r\n",Tim_Num[34],Tim_Num[35]);
				u1_printf("ELtemp_H...\r\n");
			 }
			 
       if(LCD_str[5]==0x50)   //���ù���ǿ��PWMռ�ձ�
			 {
					Elight_pwm  = LCD_str[8];
					Tim_Num[4] =	(LCD_str[7]>>8)/256;		//����ǿ��PWMռ�ձ�
					Tim_Num[5] =  LCD_str[8]    %256;
					AT24CXX_WriteOneByte(4,Tim_Num[4]);
					AT24CXX_WriteOneByte(5,Tim_Num[5]);
					u1_printf("PWM_SET=%d...\r\n",Elight_pwm);
			 }	
			 
       if(LCD_str[5]==0x25 && LCD_str[8]==0xf4) //����ϵͳ����
			 {
			    EEPROM_Data_Read();
					lcd_data[5] = 0x50;   //��  ͨ��ʱ��  //����ǿ��PWMռ�ձ���ʾ
					lcd_data[6] = ((Elight_pwm) >>8)&0x00ff;
					lcd_data[7] =  (Elight_pwm) &0x00ff;
					u3_TxData(lcd_data,8);
       }
	 }	
}
 
  


