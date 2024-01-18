#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//////////////////////////////////////////////////////////////////////////////////
 
  
//LCD重要参数集
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u8	wramcmd;		//开始写gram指令
	u8  setxcmd;		//设置x坐标指令
	u8  setycmd;		//设置y坐标指令	 
}_lcd_dev; 	  

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
//LCD的画笔颜色和背景色	   
extern u16  POINT_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色

#define  LCD_SIZE_X		 240		 
#define  LCD_SIZE_Y		 320	


//////////////////////////////////////////////////////////////////////////////////	 
//-----------------LCD端口定义---------------- 
//#define	P_contrl PAout(3) // 

//#define	LCD_RST PAout(12) //     		 RESET
//#define	LCD_CS  PBout(15) //    		 CS
//#define	LCD_RS  PAout(11) //     		 RS
//#define	LCD_WR  PAout(8)  //    		 WR
//#define	LCD_RD  PAout(7)  //    		 RD
	 
//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	 

extern u8 DFT_SCAN_DIR;


//#define LCD_RST_PIN    GPIO_Pin_12	 //RST  
#define LCD_RD_PIN     GPIO_Pin_12	 //RD   
#define LCD_WR_PIN     GPIO_Pin_13	 //WR   
#define LCD_RS_PIN     GPIO_Pin_14   //RS   
#define LCD_CS_PIN     GPIO_Pin_15	 //CS   
#define LCD_ON_PIN     GPIO_Pin_8    //LCD_ON

//#define    LCD_RST(x)  x ? GPIO_SetBits(GPIOA, LCD_RST_PIN):   GPIO_ResetBits(GPIOA, LCD_RST_PIN) //自己定义位操作函数
#define    LCD_RD(x)   x ? GPIO_SetBits(GPIOB, LCD_RD_PIN) :   GPIO_ResetBits(GPIOB, LCD_RD_PIN)
#define    LCD_WR(x)   x ? GPIO_SetBits(GPIOB, LCD_WR_PIN) :   GPIO_ResetBits(GPIOB, LCD_WR_PIN)
#define    LCD_RS(x)   x ? GPIO_SetBits(GPIOB, LCD_RS_PIN) :   GPIO_ResetBits(GPIOB, LCD_RS_PIN)
#define    LCD_CS(x)   x ? GPIO_SetBits(GPIOB, LCD_CS_PIN) :   GPIO_ResetBits(GPIOB, LCD_CS_PIN)
#define    LCD_ON(x)   x ? GPIO_SetBits(GPIOA, LCD_ON_PIN) :   GPIO_ResetBits(GPIOA, LCD_ON_PIN)

//PC0~15,作为数据口
#define DATAOUT(x) GPIOB->ODR=x; //数据输出
#define DATAIN     GPIOB->IDR;   //数据输入

//画笔颜色
#define WHITE        0x0000
#define BLACK        0xFFFF	  
#define BLUE         BLACK-0x001F  
#define BRED         BLACK-0XF81F
#define GRED 			   BLACK-0XFFE0
#define GBLUE			   BLACK-0X07FF
#define RED          BLACK-0xF800
#define MAGENTA      BLACK-0xF81F
#define GREEN        BLACK-0x07E0
#define CYAN         BLACK-0x7FFF
#define YELLOW       BLACK-0xFFE0
#define BROWN 			 BLACK-0XBC40 //棕色
#define BRRED 			 BLACK-0XFC07 //棕红色
#define GRAY  			 BLACK-0X8430 //灰色
//GUI颜色

#define DARKBLUE     BLACK-0X01CF	//深蓝色
#define LIGHTBLUE    BLACK-0X7D7C	//浅蓝色  
#define GRAYBLUE     BLACK-0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN   BLACK-0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 BLACK-0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE    BLACK-0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE       BLACK-0X2B12 //浅棕蓝色(选择条目的反色)

#define GUI_Green		   BLACK-0x1eb6
#define GUI_Yellow	   BLACK-0xfe85
#define GUI_Red				 BLACK-0xe330
#define GUI_ReadColor	 BLACK-0x4e1f
#define GUI_WriteColor BLACK-0x771a
#define BACK_Color     BLACK-0x18e6	

void LCD_Init(void);													   	//初始化
void LCD_DisplayOn(void);													//开显示
void LCD_DisplayOff(void);													//关显示
void LCD_Clear(u16 Color);	 												//清屏
void LCD_SetCursor(u16 Xpos, u16 Ypos);										//设置光标
 
void LCD_DrawPoint(u16 x,u16 y,u16 color);//画点
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);								//快速画点
u16  LCD_ReadPoint(u16 x,u16 y); 											//读点 
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);									//画圆
void LCD_Draw_Circle1(int xc, int yc,u16 c,int r, int fill);
 						//画线
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
 	   				//画矩形
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
//x y 长度  粗细  颜色  横竖线
void Paint_brush(u16 x,u16 y,u16 l,u8 t,u16 color,u8 flag);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   				//填充单色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				//填充指定颜色
 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						//显示一个数字
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//显示 数字
 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
void IO_init(void);

void opt_delay(u8 i);
void LCD_WR_REG(u8);
void LCD_WR_DATA(u16);
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u8 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);		  
void LCD_Scan_Dir(u8 dir);							//设置屏扫描方向
void LCD_Display_Dir(u8 dir);						//设置屏幕显示方向

//在指定位置显示一个字符
//void LCD_ShowChar(u16 x,u16 y,u16 For_color,u16 Bk_color, char ch);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
//void LCD_ShowChar6X12(u16 x,u16 y,u16 For_color,u16 Bk_color, char ch);
//void LCD_ShowString_n(u16  x,u16 y,u16 For_color,u16 Bk_color,char *p,u8 num);
//void LCD_ShowString6X12(u16  x,u16 y,u16 For_color,u16 Bk_color,char *p);
//显示字符串
//void LCD_ShowString(u16  x,u16 y,u16 For_color,u16 Bk_color,char *p);
void progress_bar(u16 Xpos, u16 Ypos, u16 Width, u16 Height,u16 color_f,u16 color_b, u16 bar_value);
void LCD_WriteBMP(u8 Xpos, u16 Ypos, u16 Width, u8 Height, u8 *bitmap);
void Gui_DrawFont_GBK12(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz16_num);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz16_num);
void Gui_DrawFont_GBK20(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz20_num);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz24_num);
void Gui_DrawFont_GBK32(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz32_num);
void Gui_DrawFont_GBK1632(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u16 hz1632_num);


//9320/9325 LCD寄存器  
#define R0             0x00
#define R1             0x01
#define R2             0x02
#define R3             0x03
#define R4             0x04
#define R5             0x05
#define R6             0x06
#define R7             0x07
#define R8             0x08
#define R9             0x09
#define R10            0x0A
#define R12            0x0C
#define R13            0x0D
#define R14            0x0E
#define R15            0x0F
#define R16            0x10
#define R17            0x11
#define R18            0x12
#define R19            0x13
#define R20            0x14
#define R21            0x15
#define R22            0x16
#define R23            0x17
#define R24            0x18
#define R25            0x19
#define R26            0x1A
#define R27            0x1B
#define R28            0x1C
#define R29            0x1D
#define R30            0x1E
#define R31            0x1F
#define R32            0x20
#define R33            0x21
#define R34            0x22
#define R36            0x24
#define R37            0x25
#define R40            0x28
#define R41            0x29
#define R43            0x2B
#define R45            0x2D
#define R48            0x30
#define R49            0x31
#define R50            0x32
#define R51            0x33
#define R52            0x34
#define R53            0x35
#define R54            0x36
#define R55            0x37
#define R56            0x38
#define R57            0x39
#define R59            0x3B
#define R60            0x3C
#define R61            0x3D
#define R62            0x3E
#define R63            0x3F
#define R64            0x40
#define R65            0x41
#define R66            0x42
#define R67            0x43
#define R68            0x44
#define R69            0x45
#define R70            0x46
#define R71            0x47
#define R72            0x48
#define R73            0x49
#define R74            0x4A
#define R75            0x4B
#define R76            0x4C
#define R77            0x4D
#define R78            0x4E
#define R79            0x4F
#define R80            0x50
#define R81            0x51
#define R82            0x52
#define R83            0x53
#define R96            0x60
#define R97            0x61
#define R106           0x6A
#define R118           0x76
#define R128           0x80
#define R129           0x81
#define R130           0x82
#define R131           0x83
#define R132           0x84
#define R133           0x85
#define R134           0x86
#define R135           0x87
#define R136           0x88
#define R137           0x89
#define R139           0x8B
#define R140           0x8C
#define R141           0x8D
#define R143           0x8F
#define R144           0x90
#define R145           0x91
#define R146           0x92
#define R147           0x93
#define R148           0x94
#define R149           0x95
#define R150           0x96
#define R151           0x97
#define R152           0x98
#define R153           0x99
#define R154           0x9A
#define R157           0x9D
#define R192           0xC0
#define R193           0xC1
#define R229           0xE5							  		 
#endif  
	 
	 



