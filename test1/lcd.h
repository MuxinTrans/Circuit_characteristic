/*
 * lcd.h
 *
 *  Created on: May 23, 2017
 *      Author: hailiang
 */

//用于显示动画

#ifndef LCD_H_
#define LCD_H_

#include "define.h"
#include "ascii.h"

#define  HDP  799   //水平分辨率800
#define  VDP  479   //垂直分辨率480    //设计UI界面时，现在草纸上画一个草图，并把每一个模块的坐标值标出来。
#define  HT   1055
#define  HPS  46
#define  LPS  0
#define  HPW  0
#define  VT   524
#define  VPS  23
#define  FPS  0
#define  VPW  0

enum COLOR {
	BLACK = 0x0000,
	NAVY = 0x000F,
	DGREEN = 0x03E0,
	DCYAN = 0x03EF,
	MAROOON = 0x7800,
	PURPEL = 0x780F,
	OLIVE = 0x7BE0,
	LGRAY = 0xC618,
	DGRAY = 0x7BEF,
	BLUE = 0x001F,
	GREEN = 0x07E0,
	CYAN = 0x07FF,
	RED = 0xF800,
	MAGENTA = 0xF81F,
	YELLOW = 0xFFE0,
	WHITE = 0xFFFF,
	BACKGROUND = 0xFFFF,
};

int lcdInit(void);

void lcdWriteCommand(u16 command);
void lcdWriteData(u16 data);
u16 lcdReadData(void);
void lcdSetCursor(u16 x, u16 y);
void lcdDrawPoint(u16 x, u16 y, u16 color);
u16 lcdGetPoint(u16 x, u16 y);

void lcdRectClear(u16 xs, u16 ys, u16 xe, u16 ye, u16 color);     //清屏     //将某个矩形范围内的像素全部填充成某颜色
void lcdSetLine(u8 linemode, u8 cycle, u8 duty, u8 hlpen);
void lcdDrawVerLine(u16 x, u16 ys, u16 ye, u16 color);   //画线--水平、竖直、曲线
void lcdDrawHorLine(u16 y, u16 xs, u16 xe, u16 color);
void lcdDrawLine(u16 xs, u16 ys, u16 xe, u16 ye, u16 color);

void lcdDrawPic(u16 xs, u16 ys, u16 row, u16 col, u16 fore, u16 back,
		volatile const u8*pcode);
void lcdDrawGrid(u16 xs, u16 ys, u8 row, u8 col, u8 width, u16 color);   //画网格

//字符串输出--小、大
void lcdDispStringSmall(u16 xs, u16 ys, u16 fore, u16 back, const char*str);
void lcdDispStringBig(u16 xs, u16 ys, u16 fore, u16 back, const char*str);

//整数输出
void lcdDispDecSmall(u16 xs, u16 ys, u16 fore, u16 back, int dec);
void lcdDispDecBig(u16 xs, u16 ys, u16 fore, u16 back, int dec);

//（前两行）浮点数输出   //fore & back 前景色和背景色
void lcdDispFloatSmall(u16 xs, u16 ys, u16 fore, u16 back, float flo);
void lcdDispFloatBig(u16 xs, u16 ys, u16 fore, u16 back, float flo,int weishu);
void DisplayPic(u16 x_start,u16 y_start,u16 width,u16 depth, u8 *p,u8 factor);

void lcdDispNumtable(int Num_X,int Num_Y);
void lcddrawsqur(u16 xs,u16 xe,u16 ys, u16 ye, u16 color, const char*str);
void lcdDrawRect(u16 xs, u16 ys, u16 xe, u16 ye, u16 color);

#endif /* LCD_H_ */
