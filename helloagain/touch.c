/*
 * touch.c
 *
 *  Created on: May 23, 2017
 *      Author: hailiang
 */
#define bit_length 10

#include "touch.h"
#include "lcd.h"
#include "ascii.h"

int bitnum;
int bit_array[bit_length]={0};
int bit_pointer = 0;
extern int mode, Range_w = 0;//Range_w=0-F_L|1-F_H
extern int Num_X = 50,Num_Y = 40;

int touchInit(void) {
	i2cInit();

	// normal mode
	i2cWriteReg(FT_DEVICE_ADDR, FT_DEV_MODE, 0);
	// polling mode
	i2cWriteReg(FT_DEVICE_ADDR, FT_ID_G_MODE, 0);
	// threshold
	i2cWriteReg(FT_DEVICE_ADDR, FT_ID_G_THGROUP, 22);
	// auto calibration mode
	i2cWriteReg(FT_DEVICE_ADDR, FT_ID_G_AUTO_CLB_MODE, 0);
	// period of active status 12
	i2cWriteReg(FT_DEVICE_ADDR, FT_ID_G_PERIODACTIVE, 12);

	touchIrqInit();
	return 0;
}

int touchIrqInit(void) {
	IOWR(TOUCH_IRQ_BASE, 2, 0x01);
	IOWR(TOUCH_IRQ_BASE, 3, 0x00);
	return alt_irq_register(TOUCH_IRQ_IRQ, NULL, (void *) touchIsr);
}

tcdata local = { .id = 1, .status = TOUCH_NONE, };

int touchResponse(int x_low, int x_high, int y_low, int y_high, int page) {//按键响应函数，按下变色
	if((local.now.x > x_low) && (local.now.x < x_high) && (local.now.y > y_low) && (local.now.y < y_high) && (local.status == 0))
	{
		lcdRectClear(x_low, y_low, x_high,y_high, DGRAY);
		return 1;
	}
	else if((local.now.x > x_low) && (local.now.x < x_high) && (local.now.y > y_low) && (local.now.y < y_high) && (local.status == 1))
	{
		lcdRectClear(x_low, y_low, x_high,y_high, BACKGROUND);
		return 0;
	}
	else
		return 2;
}
/*
int button_show(int x_low,int y_low, const char*str,int page) {
	u16 localX,localY;
	int x_high,y_high;
	u8 k = 0;
	y_high=y_low+40;
	x_high=x_low+50;

	if(touchResponse(x_low, x_high, y_low, y_high, page) == 1)
		return 1;
	else if(touchResponse(x_low, x_high, y_low, y_high, page) == 0)
	{
		u16 localX,localY;
		u8 k = 0;
		localX = ((x_low+x_high)/2)-18;
		localY = (y_low+y_high)/2;
		lcdDrawVerLine(x_low, y_low, y_high, BLACK);
		lcdDrawVerLine(x_high, y_low, y_high, BLACK);
		lcdDrawHorLine(y_low, x_low, x_high, BLACK);
		lcdDrawHorLine(y_high, x_low, x_high, BLACK);

		while (str[k] != 0) {
			lcdDrawPic(localX+k*8, localY, 16, 8, BLACK, WHITE, ASCII8_16[str[k]-' ']);
			k++;
		}
		return 0;
	}
	return 0;
}

void Button_selet(){//Mode = 0-测量 | 1-绘图 | 2-检错
	if(button_show()){

	}
	*/
	/*
	 	if(button_show(10,10, "model",0))
	{
		if(switch1==1)
		{
			Troubleshooting=1;
			InitAds1256();
			lcdDispStringSmall(10, 50, Dva_PINK_Deep, WHITE, "profess",20);
			switch1=0;
		}
		else if(switch1==0)
		{
			Troubleshooting=0;
			switch1=1;
			lcdDispStringSmall(10, 50, Dva_PINK_Deep, WHITE, "normal ",20);
		}
	}
	if(button_show(10,50, "Draw",0))
	{
		if(switch2==1)
		{
			switch2=0;
			draw_flag=0;
		}
		else if(switch2==0)
		{
			switch2=1;
			draw_flag=1;
		}
	}
	 */
//}

int local_x;
int local_y;
int model=0;
int status_value;
int pause1=0;
unsigned char TouchValidFlag=0;
char TouchValue='0';
void touchIsr(void* isr_context) {
	IOWR(TOUCH_IRQ_BASE, 3, 0x00);

	touchGetRaw(&local);
	TouchValidFlag=1;
	local_x=local.now.x;
	local_y=local.now.y;
	status_value=local.status;
	button_table(0);

	//触摸按键
    if(local_x >= 10 && local_x <= 105 && local_y >= 10 && local_y <= 70)
    {

    }

    else if(local_x >= 10 && local_x <= 105 && local_y >= 70 && local_y <= 130)
    {

    }

    else if(local_x >= 10 && local_x <= 105 && local_y >= 130 && local_y <= 190)
    {

    }
}

void button_table(int page){
	/****************** -1- *********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y, Num_Y+50, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_array <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=1;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y, Num_Y+50, page) == 0)
	{
		lcddrawsqur(Num_X, Num_X+50, Num_Y, Num_Y+50, BLACK, "  1");
	}
	/****************** -2- *********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y, Num_Y+50, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=2;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y, Num_Y+50, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y, Num_Y+50, BLACK, "  2");
	}

	/****************** -3- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y, Num_Y+50, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=3;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y, Num_Y+50, page) == 0)
	{
		lcddrawsqur(Num_X+100,Num_X+150,Num_Y,Num_Y+50,BLACK, "  3");
	}

	/****************** -4- *********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y+50, Num_Y+100, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=4;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y+50, Num_Y+100, page) == 0)
	{
		lcddrawsqur(Num_X,Num_X+50,Num_Y+50,Num_Y+100,BLACK, "  4");
	}

	/****************** -5- *********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y+50, Num_Y+100, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=5;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y+50, Num_Y+100, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y+50, Num_Y+100, BLACK, "  5");
	}

	/****************** -6- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y+50, Num_Y+100, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=6;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y+50, Num_Y+100, page) == 0)
	{
		lcddrawsqur(Num_X+100, Num_X+150, Num_Y+50, Num_Y+100, BLACK, "  6");
	}

	/****************** -7- *********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y+100, Num_Y+150, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=7;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y+100, Num_Y+150, page) == 0)
	{
		lcddrawsqur(Num_X, Num_X+50, Num_Y+100, Num_Y+150, BLACK, "  7");
	}

	/****************** -8- *********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y+100, Num_Y+150, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=8;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y+100, Num_Y+150, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y+100, Num_Y+150, BLACK, "  8");
	}

	/****************** -9- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y+100, Num_Y+150, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=9;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y+100, Num_Y+150, page) == 0)
	{
		lcddrawsqur(Num_X+100, Num_X+150, Num_Y+100, Num_Y+150, BLACK, "  9");
	}

	/****************** -ok- *********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y+150, Num_Y+200, page)==1)
	{
		/*按键功能定义区——Begin*/

		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y+150, Num_Y+200, page)==0)
	{
		lcddrawsqur(Num_X, Num_X+50, Num_Y+150, Num_Y+200, BLACK, " ok");
	}

	/****************** -0- **********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y+150, Num_Y+200, page) == 1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=0;
			if(Range_w)
				lcdDispDecSmall(130+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(130+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y+150, Num_Y+200, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y+150, Num_Y+200, BLACK, "  0");
	}

	/***************** -del- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y+150, Num_Y+200, page)==1)
	{
		/*按键功能定义区——Begin*/
		if(bit_pointer >= 1)
		{
			bit_array[bit_pointer]=0;//=0?=' '?
			if(Range_w)
				lcdRectClear(130+(bit_pointer*9), 425, Num_X+(bit_pointer*9)+7, 450, BACKGROUND);
			else
				lcdRectClear(130+(bit_pointer*9), 385, Num_X+(bit_pointer*9)+7, 405, BACKGROUND);
			bit_pointer--;
		}
		/*按键功能定义区——End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y+150, Num_Y+200, page) == 0)
	{
		lcddrawsqur(Num_X+100, Num_X+150, Num_Y+150, Num_Y+200, BLACK, " del");
	}

	/***************** -mode- ********************/

	/***************** -F_L- *********************/

	/***************** -F_H- *********************/
}

const u8 tpid[5] = { FT_TP1, FT_TP2, FT_TP3, FT_TP4, FT_TP5 };
void touchGetData(tcdata *data) {

	if (local.status == TOUCH_DOWN) {
		data->status = TOUCH_DOWN;
		data->now = local.now;

		local.status = TOUCH_CONTACT;
	} else if (local.status == TOUCH_CONTACT) {
		touchGetRaw(data);
	} else if (local.status == TOUCH_UP) {
		data->status = TOUCH_UP;
		data->now = local.now;

		local.status = TOUCH_NONE;
	} else {
		data->status = TOUCH_NONE;
	}
}


void touchGetRaw(tcdata *data) {
	u8 buff[4] = { 0 };

	i2cReadMulti(FT_DEVICE_ADDR, tpid[data->id - 1], buff, 4);

	data->status = (buff[0] >> 6) & 0x03;
	data->old = data->now;
	data->now.y = ((buff[0] & 0x0f) << 8) + buff[1];
	data->now.x = ((buff[2] & 0x0f) << 8) + buff[3];
}

u8 touchGetGesture(void) {
	return i2cReadReg(FT_DEVICE_ADDR, FT_GEST_ID);
}

