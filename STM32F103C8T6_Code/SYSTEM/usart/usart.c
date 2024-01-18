#include "sys.h"
#include "usart.h"	
#include "delay.h"
#include "stdarg.h"		//������Ҫ��ͷ�ļ� 
#include "string.h"     //������Ҫ��ͷ�ļ�
#include "lcd.h"
#include "main.h"         //������Ҫ��ͷ�ļ�
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}


/*-------------------------------------------------*/
/*������������1 printf����                         */
/*��  ����char* fmt,...  ��ʽ������ַ����Ͳ���    */
/*����ֵ����                                       */
/*-------------------------------------------------*/

__align(8) char Usart1_TxBuff[USART1_TXBUFF_SIZE];  

void u1_printf(unsigned char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(Usart1_TxBuff,(char*)fmt,ap); 
	va_end(ap);	
	
	length=strlen((const char*)Usart1_TxBuff);		
	while((USART1->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		USART1->DR = Usart1_TxBuff[i];
		while((USART1->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*������������2���ͻ������е�����                  */
/*��  ����data������                               */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u1_TxData(unsigned char *data,char len)
{
	int	i;	
	while((USART1->SR&0X40)==0);
	for(i = 0;i < len;i ++){			
		USART1->DR = data[i];
		while((USART1->SR&0X40)==0);	
	}
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		USART_RX_BUF[USART_RX_STA]=Res ;
		USART_RX_STA++;
		if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  		 
	} 
} 


unsigned Checksum(unsigned char*pMsg,unsigned char len)
{
	unsigned char result = 0;
	while (len--)
	{
		result += *pMsg++;
	}
	result &= 0x00ff;
	return result;
}

//void Get_Height(void)
//{
//	unsigned char Data_Heigh[5]={0x55,0xaa,0x01,0x01,0x01};

//	u1_printf(Data_Heigh);
//	delay_ms(2000);
//	if(USART_RX_BUF[6] == Checksum(USART_RX_BUF,6))
//	{
//		Sensor.Height = (USART_RX_BUF[4]<<8)|USART_RX_BUF[5];
// 

//		memset(USART_RX_BUF,0,USART_RX_STA); 
//		USART_RX_STA=0;
//	}
//	else
//	{
//		memset(USART_RX_BUF,0,USART_RX_STA);
//		USART_RX_STA=0;
//	}
//}

//void Get_Temp(void)
//{
//	unsigned char Data_Temp[5] ={0x55,0xaa,0x01,0x02,0x02};

//	u1_printf(Data_Temp);
//	delay_ms(2000);
//	if(USART_RX_BUF[6] == Checksum(USART_RX_BUF,6))
//	{
//		Sensor.Temperature = (USART_RX_BUF[4]<<8)|USART_RX_BUF[5];
//	
//		memset(USART_RX_BUF,0,USART_RX_STA);
//		USART_RX_STA=0;
//	}
//	else
//	{
//		memset(USART_RX_BUF,0,USART_RX_STA);
//		USART_RX_STA=0;
//	}
//}

#endif

