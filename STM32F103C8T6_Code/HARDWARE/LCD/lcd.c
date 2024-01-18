
#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h"	 
#include "delay.h"	   
//////////////////////////////////////////////////////////////////////////////////	 
u8 DFT_SCAN_DIR;
//////////////////////////////////////////////////////////////////////////////////	 
				 
//LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 POINT_COLOR=0x0000;	//������ɫ
u16 BACK_COLOR=0xFFFF;  //����ɫ 

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;
		   
//д�Ĵ�������
//regval:�Ĵ���ֵ
void LCD_WR_REG(u8 regval)
{ 
  u16 DAT;

  LCD_CS(0);
	LCD_RS(0); //DATA   1  COMMAND	0
//	opt_delay(5);
	
	DAT = DATAIN;
	DAT = DAT & 0xff00;	
	DATAOUT(regval | DAT);
//	printf("regval=%x\r\n",regval | DAT);
	LCD_WR(0);
	LCD_WR(1);
//	opt_delay(5);
	LCD_CS(1);
}
//дLCD����
//data:Ҫд���ֵ
void LCD_WR_DATA(u16 data)
{				
  u16 DAT;

  LCD_CS(0);
	LCD_RS(1);
//	opt_delay(5);
	
	DAT = DATAIN;
	DAT = DAT & 0xff00;
	DATAOUT(DAT | data);
//	printf("data=%x\r\n",DAT | data);
	LCD_WR(0);
	LCD_WR(1);
//	opt_delay(5);
	LCD_CS(1);	
 
}					   
//д�Ĵ���
//LCD_Reg:�Ĵ�����ַ
//LCD_RegValue:Ҫд�������
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}	      
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD_WR_REG(lcddev.wramcmd);
}	 
//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(u16 RGB_Code)
{		
	u16 DAT,DAT_H,DAT_L;

	LCD_CS(0);
	LCD_RS(1); 
	LCD_RD(1); 
	DAT = DATAIN;
	DAT = DAT & 0xff00;
	DAT_H = (RGB_Code>>8) & 0x00FF;
	
	DATAOUT(DAT_H | DAT);
	LCD_WR(0);
	LCD_WR(1);
//	opt_delay(5);
 
	DAT_L = RGB_Code & 0x00FF;
	DATAOUT(DAT_L | DAT);
	LCD_WR(0);
	LCD_WR(1);	
	LCD_CS(1);
 
} 

//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(u8 i)
{
	while(i--);
}
  
//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(Xpos>>8); 
	LCD_WR_DATA(Xpos&0XFF);	
//  LCD_WR_DATA((lcddev.width-1)>>8);
//	LCD_WR_DATA((lcddev.width-1)&0XFF);
	
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(Ypos>>8); 
	LCD_WR_DATA(Ypos&0XFF);
//	LCD_WR_DATA((lcddev.height-1)>>8);
//	LCD_WR_DATA((lcddev.height-1)&0XFF);
  
	
} 		 
//����LCD���Զ�ɨ�跽��
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
//����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341��IC�Ѿ�ʵ�ʲ���	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u8 dirreg=0;
//	u16 temp;  
	switch(dir)//����ת��
	{
		case 0:dir=6;break;
		case 1:dir=7;break;
		case 2:dir=4;break;
		case 3:dir=5;break;
		case 4:dir=1;break;
		case 5:dir=0;break;
		case 6:dir=3;break;
		case 7:dir=2;break;	     
	}
	switch(dir)
	{
		case L2R_U2D://������,���ϵ���
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	dirreg=0X36; 
  regval|=0x00;	
	LCD_WriteReg(dirreg,regval);
			
  LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(0);LCD_WR_DATA(0);
	LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
	
}      
 
//����
//x,y:����
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	LCD_WriteRAM(color);
}

//����LCD��ʾ����6804��֧�ֺ�����ʾ��
//dir:0,������1,����
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//����
	{
		lcddev.dir=0;	//����
		lcddev.width=240;
		lcddev.height=320;

		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
    DFT_SCAN_DIR=U2D_R2L;		

	}else 				//����
	{	  				
		lcddev.dir=1;	//����
		lcddev.width=320;
		lcddev.height=240;

		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
    DFT_SCAN_DIR=L2R_U2D;		
		
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}
//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ������ILI93XXҺ��,�������������ǻ���ILI9320��!!!
//�������ͺŵ�����оƬ��û�в���! 
void LCD_Init(void)
{ 										  
 
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); //ʹ��PORTB,Cʱ�Ӻ�AFIOʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//����SWD��ʧ��JTAG
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7 | GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB
  GPIO_Write(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7 | GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //��GPIOB  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	   
	GPIO_Init(GPIOA, &GPIO_InitStructure); //GPIOB
  GPIO_Write(GPIOA, GPIO_Pin_8);  //��GPIOB  
 
	//  //LCD��λ 
  LCD_ON(1);
//	LCD_CS(0);  //��Ƭѡʹ��
  LCD_RD(1);
//************* Start Initial Sequence **********//
//----------------------------------------end Reset Sequence---------------------------------------//
//----------------------------------display control setting----------------------------------------//
	LCD_WR_REG(0xfe);
	LCD_WR_REG(0xef);
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0x00);//0X40
	LCD_WR_REG(0x3a);
	LCD_WR_DATA(0x05);
	LCD_WR_REG(0xe8);
	LCD_WR_DATA(0x12);
	LCD_WR_DATA(0x22);
	//----------------------------------end display control setting--------------------------------//
	//----------------------------------Power Control Registers Initial--------------------------------//
	LCD_WR_REG(0xe3);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x04);
	LCD_WR_REG(0xa5);
	LCD_WR_DATA(0x40);
	LCD_WR_DATA(0x40);
	LCD_WR_REG(0xa4);
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x44);
	LCD_WR_REG(0xab);
	LCD_WR_DATA(0x08);
	LCD_WR_REG(0xaa);
	LCD_WR_DATA(0x88);
	LCD_WR_DATA(0x88);
	LCD_WR_REG(0xae);
	LCD_WR_DATA(0x0b);
	LCD_WR_REG(0xac);
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xaf);
	LCD_WR_DATA(0x77);
	LCD_WR_REG(0xad);
	LCD_WR_DATA(0x77);
	//---------------------------------display window 240X320-------------------------------------//
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);
	LCD_WR_REG(0x2b);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2c);
	//------------------------------------end display window ------------------------------------------//
	//----------------------------------------gamma setting----------------------------------------------//
	LCD_WR_REG(0xF0);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x09);
	LCD_WR_DATA(0x13);
	LCD_WR_DATA(0x0E);
	LCD_WR_REG(0xF1);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x02);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x1C);
	LCD_WR_DATA(0x10);
	LCD_WR_REG(0xF2);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x08);
	LCD_WR_DATA(0x39);
	LCD_WR_DATA(0x04);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x49);
	LCD_WR_REG(0xF3);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x0A);
	LCD_WR_DATA(0x41);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x03);
	LCD_WR_DATA(0x4F);
	LCD_WR_REG(0xF4);
	LCD_WR_DATA(0x0F);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x16);
	LCD_WR_DATA(0x1D);
	LCD_WR_DATA(0x20);
	LCD_WR_DATA(0x0F);
	LCD_WR_REG(0xF5);
	LCD_WR_DATA(0x05);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x1C);
	LCD_WR_DATA(0x1F);
	LCD_WR_DATA(0x0F);
	//------------------------------------end gamma setting------------------------------------------//
	LCD_WR_REG(0x11);
	delay_ms(120); 
	LCD_WR_REG(0x29);
	LCD_WR_REG(0x2c); 

	LCD_Display_Dir(0);		 	//Ĭ��Ϊ����
	LCD_Clear(BLACK);
//	while(1); 
}  
//��������
//color:Ҫ���������ɫ
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 	//�õ��ܵ���
  LCD_SetCursor(0x00,0x00);	//���ù��λ��
	LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
	
	for(index=0;index<totalpoint;index++)
	{
		LCD_WriteRAM(color);	   
	}
	
}  
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	xlen=ex-sx+1;	   
	for(i=sy;i<=ey;i++)
	{
	 	LCD_SetCursor(sx,i);      				//���ù��λ�� 
		LCD_WriteRAM_Prepare();     			//��ʼд��GRAM	  
		for(j=0;j<xlen;j++)LCD_WriteRAM(color);	//���ù��λ�� 	    
	}
}  
//��ָ�����������ָ����ɫ��			 
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 		//�õ����Ŀ��
	height=ey-sy+1;		//�߶�
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM
		for(j=0;j<width;j++)LCD_WriteRAM(color[i*height+j]);//д������ 
	}	  
}  
//����
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
//		LCD_DrawPoint(uRow,uCol);//����
		LCD_DrawPoint(uRow,uCol,color);		
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

//x y ����  ��ϸ  ��ɫ  ������
void Paint_brush(u16 x,u16 y,u16 l,u8 t,u16 color,u8 flag)
{
	u8 i;
	if(!flag)
	{
		for(i=0;i<t;i++)
		LCD_Fill(x,y,x+l,y+i,color);//����
	}
	else
	{
		for(i=0;i<t;i++)
	 LCD_Fill(x,y,x+i,y+l,color);//����
	}
}

//���ܣ�    8�Գ��Ի�Բ�㷨(�ڲ�����)
//���������(xc,yc) :Բ��������
// 			(x,y):��������Բ�ĵ�����
//         	c:������ɫ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************  
void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
{
	LCD_DrawPoint(xc + x, yc + y, c);

	LCD_DrawPoint(xc - x, yc + y, c);

	LCD_DrawPoint(xc + x, yc - y, c);

	LCD_DrawPoint(xc - x, yc - y, c);

	LCD_DrawPoint(xc + y, yc + x, c);

	LCD_DrawPoint(xc - y, yc + x, c);

	LCD_DrawPoint(xc + y, yc - x, c);

	LCD_DrawPoint(xc - y, yc - x, c);
}

//���ܣ�    ��ָ��λ�û�һ��ָ����С��Բ(���)
//���������(xc,yc) :Բ��������
//         	c:������ɫ
//		 	r:Բ�뾶
//		 	fill:����жϱ�־��1-��䣬0-�����
//����ֵ��  ��
//�޸ļ�¼����
void LCD_Draw_Circle1(int xc, int yc,u16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill) 
	{
		// �����䣨��ʵ��Բ��
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				_draw_circle_8(xc, yc, x, yi, c);

			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	} else 
	{
		// �������䣨������Բ��
		while (x <= y) {
			_draw_circle_8(xc, yc, x, y, c);
			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}

//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b,color);             //5
 		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-a,y0+b,color);             //1       
 		LCD_DrawPoint(x0-b,y0+a,color);             
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
  		LCD_DrawPoint(x0-b,y0-a,color);             //7     	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 														  

//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24��32��40
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		     if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==20)temp=asc2_2010[num][t];	//����2412����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else if(size==32)temp=asc2_3216[num][t];	//����2412����
		else if(size==40)temp=asc2_4020[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++) 
		{			    
			if(temp&0x80)
			{
		 		LCD_SetCursor(x, y);
				LCD_WriteRAM_Prepare();
	      LCD_WriteRAM(POINT_COLOR);  //textcolor
			}
//				LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)
			{
			  LCD_SetCursor(x, y);
				LCD_WriteRAM_Prepare();
	      LCD_WriteRAM(BACK_COLOR);  //textcolor
			}
//				LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
} 



 


//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
 

//���ô���
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{   
	width=sx+width-1;
	height=sy+height-1;

	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(sx>>8);  
	LCD_WR_DATA(sx&0XFF);	  
	LCD_WR_DATA(width>>8);   
	LCD_WR_DATA(width&0XFF);   
	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(sy>>8);   
	LCD_WR_DATA(sy&0XFF);  
	LCD_WR_DATA(height>>8);   
	LCD_WR_DATA(height&0XFF);  
}

//progress_bar(10,200,100,20,BLUE,RED, 60);//����  X Y ���W   �߶�H 
void progress_bar(u16 Xpos, u16 Ypos, u16 Width, u16 Height,u16 color_f,u16 color_b, u16 bar_value)
{
	u8 temp;
	temp = bar_value*Width/100;
	LCD_Fill( Xpos, Ypos, temp+Xpos+1, Ypos+Height, color_f);
	LCD_Fill( temp+Xpos+1, Ypos, Xpos+Width, Ypos+Height, color_b);
}


/************************************************************************************
* ��    �ƣ�void LCD_WriteBMP(u8 Xpos, u16 Ypos, u8 Height, u16 Width, u8 *bitmap)
* ��    �ܣ���ͼ
* ��ڲ�����u8 Xpos   : ��ʾͼ��X��
*           u16 Ypos  ����ʾͼ��Y��
*			u8 Height ����ʾͼ�ĳ�
*			u16 Width ����ʾͼ�Ŀ�
*			u8 *bitmap: ��ʾͼ����ָ��
* ���ڲ�������
* ˵    ������ʾһ��ͼƬ
* ���÷�����
************************************************************************************/
void LCD_WriteBMP(u8 Xpos, u16 Ypos, u16 Width, u8 Height, u8 *bitmap)
{
	u16 x,y;
  u16 *bitmap_ptr = (u16 *)bitmap;
	
  for(y = 0; y < Height; y++)
  {	
		for(x = 0; x < Width; x++)
		{
		LCD_SetCursor(Xpos+x, Ypos+y);
		LCD_WriteRAM_Prepare();	//��ʼд��GRAM			
	  LCD_WriteRAM(*bitmap_ptr++);
		}
  }
}

//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}


 
void LCD_ShowString_n(u16  x,u16 y,u16 For_color,u16 Bk_color,char *p,u8 num)
{ 	
	u8 count=0;
	u8 i;
   for(i=0;i<num;i++)
   {       
     LCD_ShowChar(x,y,For_color,Bk_color,*p);
     x+=8;
     p++;
		 count++;
		 if(count==27)
		 {
		   y+=12;
			 x=10;
			 count=0;
		 }
   } 
}



void Gui_DrawFont_GBK12(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz12_num)
{
	unsigned char i,j;
	unsigned short k;
	//x0 = x ;
	u8 count=0;
	
	while(*s) 
	{	
		if((*s) >= 128) 
		{		
			for (k=0;k<hz12_num;k++) 
			{
			  if ((hz12[k].Index[0]==*(s))&&(hz12[k].Index[1]==*(s+1)))					
			  { 
				    for(i=0;i<12;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz12[k].Msk[i*2]&(0x80>>j))	
										LCD_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) 
										LCD_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz12[k].Msk[i*2+1]&(0x80>>j))	
										LCD_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) 
										LCD_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=12;
		}
		else
		s+=1; 
		
		 count++;
		 if(count==18)
		 {
		   y+=12;
			 x=20;
			 count=0;
		 }
	}
}

void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz16_num)
{
	unsigned char i,j;
	unsigned short k;
	//x0 = x ;

	while(*s) 
	{	
		if((*s) >= 128) 
		{		
			for (k=0;k<hz16_num;k++) 
			{
			  if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<16;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2]&(0x80>>j))	
										LCD_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) 
										LCD_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2+1]&(0x80>>j))	
										LCD_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) 
										LCD_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
		}
		else
		s+=1; 
	}
}

void Gui_DrawFont_GBK20(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz20_num)
{
	unsigned char i,j;
	unsigned short k;
	//x0 = x ;

	while(*s) 
	{	
		if((*s) >= 128) 
		{		
			for (k=0;k<hz20_num;k++) 
			{
			  if ((hz20[k].Index[0]==*(s))&&(hz20[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<20;i++)
				    {
							for(j=0;j<8;j++) 
								{
										if(hz20[k].Msk[i*3]&(0x80>>j))	
											LCD_DrawPoint(x+j,y+i,fc);
									else {
										if (fc!=bc) 
											LCD_DrawPoint(x+j,y+i,bc);
									}
								}
							for(j=0;j<8;j++) 
								{
										if(hz20[k].Msk[i*3+1]&(0x80>>j))	
											LCD_DrawPoint(x+j+8,y+i,fc);
									else 
									{
										if (fc!=bc) 
											LCD_DrawPoint(x+j+8,y+i,bc);
									}
								}
							for(j=0;j<8;j++) 
								{
										if(hz20[k].Msk[i*3+2]&(0x80>>j))	
											LCD_DrawPoint(x+j+16,y+i,fc);
									else {
										if (fc!=bc) 
											LCD_DrawPoint(x+j+16,y+i,bc);
									}
								}
				    }
				}
			  }
			s+=2;x+=20;
		}
		else
		s+=1; 
	}
}

void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz24_num)
{
	unsigned char i,j;
	unsigned short k;
	//x0 = x ;

	while(*s) 
	{	
		if((*s) >= 128) 
		{		
			for (k=0;k<hz24_num;k++) 
			{
			  if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<24;i++)
				    {
							for(j=0;j<8;j++) 
								{
										if(hz24[k].Msk[i*3]&(0x80>>j))	
											LCD_DrawPoint(x+j,y+i,fc);
									else {
										if (fc!=bc) 
											LCD_DrawPoint(x+j,y+i,bc);
									}
								}
							for(j=0;j<8;j++) 
								{
										if(hz24[k].Msk[i*3+1]&(0x80>>j))	
											LCD_DrawPoint(x+j+8,y+i,fc);
									else 
									{
										if (fc!=bc) 
											LCD_DrawPoint(x+j+8,y+i,bc);
									}
								}
							for(j=0;j<8;j++) 
								{
										if(hz24[k].Msk[i*3+2]&(0x80>>j))	
											LCD_DrawPoint(x+j+16,y+i,fc);
									else {
										if (fc!=bc) 
											LCD_DrawPoint(x+j+16,y+i,bc);
									}
								}
				    }
				}
			  }
			s+=2;x+=24;
		}
		else
		s+=1; 
	}
}

void Gui_DrawFont_GBK32(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz32_num)
{
	unsigned char i,j;
	unsigned short k;
	//x0 = x ;

	while(*s) 
	{	
		if((*s) >= 128) 
		{		
			for (k=0;k<hz32_num;k++) 
			{
			  if ((hz32[k].Index[0]==*(s))&&(hz32[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<32;i++)
				    {
							for(j=0;j<8;j++) 
								{
										if(hz32[k].Msk[i*4]&(0x80>>j))	
											LCD_DrawPoint(x+j,y+i,fc);
										
									else {
										if (fc!=bc) 
											LCD_DrawPoint(x+j,y+i,bc);
									}
								}
							 for(j=0;j<8;j++) 
								{
										if(hz32[k].Msk[i*4+1]&(0x80>>j))	
											LCD_DrawPoint(x+j+8,y+i,fc);
									else 
									{
										if (fc!=bc) 
											LCD_DrawPoint(x+j+8,y+i,bc);
									}
								}
								
							for(j=0;j<8;j++) 
								{
										if(hz32[k].Msk[i*4+2]&(0x80>>j))	
											LCD_DrawPoint(x+j+16,y+i,fc);
									else 
									{
										if (fc!=bc) 
											LCD_DrawPoint(x+j+16,y+i,bc);
									}
								}
							for(j=0;j<8;j++) 
								{
										if(hz32[k].Msk[i*4+3]&(0x80>>j))	
											LCD_DrawPoint(x+j+24,y+i,fc);
									else 
									{
										if (fc!=bc) 
											LCD_DrawPoint(x+j+24,y+i,bc);
									}
								}	
								
				    }
				}
			  }
			s+=2;x+=32;
		}
		else
		s+=1; 
		
	}
}
 

void Gui_DrawFont_GBK1632(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz1632_num)
{
	unsigned char i,j;
	unsigned short k;
	//x0 = x ;

	while(*s) 
	{	
		if((*s) >= 128) 
		{		
			for (k=0;k<hz1632_num;k++) 
			{
			  if ((hz1632[k].Index[0]==*(s))&&(hz1632[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<32;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz1632[k].Msk[i*2]&(0x80>>j))	
										LCD_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) 
										LCD_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz1632[k].Msk[i*2+1]&(0x80>>j))	
										LCD_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) 
										LCD_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
		}
		else
		s+=1; 
		
	}
}


