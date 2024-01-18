
#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h"	 
#include "delay.h"	   
//////////////////////////////////////////////////////////////////////////////////	 
u8 DFT_SCAN_DIR;
//////////////////////////////////////////////////////////////////////////////////	 
				 
//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;
		   
//写寄存器函数
//regval:寄存器值
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
//写LCD数据
//data:要写入的值
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
//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}	      
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD_WR_REG(lcddev.wramcmd);
}	 
//LCD写GRAM
//RGB_Code:颜色值
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

//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
	while(i--);
}
  
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
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
//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341等IC已经实际测试	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u8 dirreg=0;
//	u16 temp;  
	switch(dir)//方向转换
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
		case L2R_U2D://从左到右,从上到下
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://从下到上,从右到左
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
 
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD_WriteRAM(color);
}

//设置LCD显示方向（6804不支持横屏显示）
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;

		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
    DFT_SCAN_DIR=U2D_R2L;		

	}else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;

		lcddev.wramcmd=0X2C;
		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
    DFT_SCAN_DIR=L2R_U2D;		
		
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}
//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试! 
void LCD_Init(void)
{ 										  
 
 	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE); //使能PORTB,C时钟和AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);//开启SWD，失能JTAG
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7 | GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIOB
  GPIO_Write(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7 | GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //将GPIOB  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	   
	GPIO_Init(GPIOA, &GPIO_InitStructure); //GPIOB
  GPIO_Write(GPIOA, GPIO_Pin_8);  //将GPIOB  
 
	//  //LCD复位 
  LCD_ON(1);
//	LCD_CS(0);  //打开片选使能
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

	LCD_Display_Dir(0);		 	//默认为竖屏
	LCD_Clear(BLACK);
//	while(1); 
}  
//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 	//得到总点数
  LCD_SetCursor(0x00,0x00);	//设置光标位置
	LCD_WriteRAM_Prepare();     //开始写入GRAM	
	
	for(index=0;index<totalpoint;index++)
	{
		LCD_WriteRAM(color);	   
	}
	
}  
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	xlen=ex-sx+1;	   
	for(i=sy;i<=ey;i++)
	{
	 	LCD_SetCursor(sx,i);      				//设置光标位置 
		LCD_WriteRAM_Prepare();     			//开始写入GRAM	  
		for(j=0;j<xlen;j++)LCD_WriteRAM(color);	//设置光标位置 	    
	}
}  
//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 		//得到填充的宽度
	height=ey-sy+1;		//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)LCD_WriteRAM(color[i*height+j]);//写入数据 
	}	  
}  
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
//		LCD_DrawPoint(uRow,uCol);//画点
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
//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

//x y 长度  粗细  颜色  横竖线
void Paint_brush(u16 x,u16 y,u16 l,u8 t,u16 color,u8 flag)
{
	u8 i;
	if(!flag)
	{
		for(i=0;i<t;i++)
		LCD_Fill(x,y,x+l,y+i,color);//横线
	}
	else
	{
		for(i=0;i<t;i++)
	 LCD_Fill(x,y,x+i,y+l,color);//竖线
	}
}

//功能：    8对称性画圆算法(内部调用)
//输入参数：(xc,yc) :圆中心坐标
// 			(x,y):光标相对于圆心的坐标
//         	c:填充的颜色
//返回值：  无
//修改记录：无
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

//功能：    在指定位置画一个指定大小的圆(填充)
//输入参数：(xc,yc) :圆中心坐标
//         	c:填充的颜色
//		 	r:圆半径
//		 	fill:填充判断标志，1-填充，0-不填充
//返回值：  无
//修改记录：无
void LCD_Draw_Circle1(int xc, int yc,u16 c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;


	if (fill) 
	{
		// 如果填充（画实心圆）
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
		// 如果不填充（画空心圆）
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

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
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
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 														  

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24、32、40
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		     if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==20)temp=asc2_2010[num][t];	//调用2412字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else if(size==32)temp=asc2_3216[num][t];	//调用2412字体
		else if(size==40)temp=asc2_4020[num][t];	//调用2412字体
		else return;								//没有的字库
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
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
} 



 


//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
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
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
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
 

//设置窗口
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

//progress_bar(10,200,100,20,BLUE,RED, 60);//坐标  X Y 宽度W   高度H 
void progress_bar(u16 Xpos, u16 Ypos, u16 Width, u16 Height,u16 color_f,u16 color_b, u16 bar_value)
{
	u8 temp;
	temp = bar_value*Width/100;
	LCD_Fill( Xpos, Ypos, temp+Xpos+1, Ypos+Height, color_f);
	LCD_Fill( temp+Xpos+1, Ypos, Xpos+Width, Ypos+Height, color_b);
}


/************************************************************************************
* 名    称：void LCD_WriteBMP(u8 Xpos, u16 Ypos, u8 Height, u16 Width, u8 *bitmap)
* 功    能：绘图
* 入口参数：u8 Xpos   : 显示图的X轴
*           u16 Ypos  ：显示图的Y轴
*			u8 Height ：显示图的长
*			u16 Width ：显示图的宽
*			u8 *bitmap: 显示图数据指针
* 出口参数：无
* 说    明：显示一个图片
* 调用方法：
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
		LCD_WriteRAM_Prepare();	//开始写入GRAM			
	  LCD_WriteRAM(*bitmap_ptr++);
		}
  }
}

//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
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


