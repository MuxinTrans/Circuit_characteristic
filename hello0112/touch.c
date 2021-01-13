/*
 * touch.c
 *
 *  Created on: May 23, 2017
 *      Author: hailiang
 */
#define bit_length 8

#include "touch.h"
#include "lcd.h"
#include "ascii.h"

int bitnum;
int bit_array[bit_length]={0};
int bit_pointer = 0;
int num_output = 0;
extern int mode = 0, Range_w = 0;//Range_w=0-F_L|1-F_H
extern int Num_X = 50,Num_Y = 40;
extern int Range_up_data = 0,Range_down_data = 0;
extern int Troubleshooting,draw_flag,ifVDC = 0;

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

int touchResponse(int x_low, int x_high, int y_low, int y_high, int page) {//������Ӧ���������±�ɫ
	if((local.now.x > x_low) && (local.now.x < x_high) && (local.now.y > y_low) && (local.now.y < y_high) && (local.status == 0))
	{
		lcdRectClear(x_low, y_low, x_high,y_high, DGRAY);
//		printf("Touch Response!\n");
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

unsigned char TouchValidFlag=0;
char TouchValue='0';
void touchIsr(void* isr_context) {
	IOWR(TOUCH_IRQ_BASE, 3, 0x00);

	touchGetRaw(&local);
	TouchValidFlag=1;
	button_table(0);
}

void button_table(int page){
	/****************** -1- *********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y, Num_Y+50, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=1;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y, Num_Y+50, page) == 0)
	{
		lcddrawsqur(Num_X, Num_X+50, Num_Y, Num_Y+50, BLACK, "  1");
	}
	/****************** -2- *********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y, Num_Y+50, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=2;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y, Num_Y+50, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y, Num_Y+50, BLACK, "  2");
	}

	/****************** -3- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y, Num_Y+50, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=3;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y, Num_Y+50, page) == 0)
	{
		lcddrawsqur(Num_X+100,Num_X+150,Num_Y,Num_Y+50,BLACK, "  3");
	}

	/****************** -4- *********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y+50, Num_Y+100, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=4;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y+50, Num_Y+100, page) == 0)
	{
		lcddrawsqur(Num_X,Num_X+50,Num_Y+50,Num_Y+100,BLACK, "  4");
	}

	/****************** -5- *********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y+50, Num_Y+100, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=5;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y+50, Num_Y+100, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y+50, Num_Y+100, BLACK, "  5");
	}

	/****************** -6- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y+50, Num_Y+100, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=6;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y+50, Num_Y+100, page) == 0)
	{
		lcddrawsqur(Num_X+100, Num_X+150, Num_Y+50, Num_Y+100, BLACK, "  6");
	}

	/****************** -7- *********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y+100, Num_Y+150, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=7;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y+100, Num_Y+150, page) == 0)
	{
		lcddrawsqur(Num_X, Num_X+50, Num_Y+100, Num_Y+150, BLACK, "  7");
	}

	/****************** -8- *********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y+100, Num_Y+150, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=8;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y+100, Num_Y+150, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y+100, Num_Y+150, BLACK, "  8");
	}

	/****************** -9- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y+100, Num_Y+150, page) == 1)
	{
		/*�������ܶ���������Begin*/
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=9;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y+100, Num_Y+150, page) == 0)
	{
		lcddrawsqur(Num_X+100, Num_X+150, Num_Y+100, Num_Y+150, BLACK, "  9");
	}

	/****************** -ok- *********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y+150, Num_Y+200, page)==1)
	{
		/*�������ܶ���������Begin*/
		for(int i=1; i <= bit_pointer; i++)
		{
			num_output = num_output+bit_array[i]*pow(10,bit_pointer-i);
			bit_array[i]=0;
		}
		bit_pointer = 0;
		if(Range_w){
			if(num_output > 10000000){
				num_output = 10000000;
				lcdRectClear(126, 425, 198, 437, BACKGROUND);
				lcdDispDecSmall(129, 425, BLACK, WHITE, num_output);
			}
			Range_up_data = num_output;
			num_output = 0;
		}
		else{
			if(num_output < 500){
				num_output = 500;
				lcdRectClear(126, 385, 198, 397, BACKGROUND);
				lcdDispDecSmall(129, 385, BLACK, WHITE, num_output);
			}
			Range_down_data = num_output;
			num_output = 0;
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y+150, Num_Y+200, page)==0)
	{
		lcddrawsqur(Num_X, Num_X+50, Num_Y+150, Num_Y+200, BLACK, " ok");
	}

	/****************** -0- **********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y+150, Num_Y+200, page) == 1)
	{
		/*�������ܶ���������Begin*/
		ifVDC = (ifVDC+1)%2;
		if((bit_pointer == 0) && (Range_w)){
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		}
		else if((bit_pointer == 0) && (!Range_w)){
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		}

		if(bit_pointer <= bit_length-1)
		{
			bit_pointer++;
			bit_array[bit_pointer]=0;
			if(Range_w)
				lcdDispDecSmall(120+(bit_pointer*9), 425, BLACK, WHITE, bit_array[bit_pointer]);
			else
				lcdDispDecSmall(120+(bit_pointer*9), 385, BLACK, WHITE, bit_array[bit_pointer]);
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y+150, Num_Y+200, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y+150, Num_Y+200, BLACK, "  0");
	}

	/***************** -clc- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y+150, Num_Y+200, page)==1)
	{
		/*�������ܶ���������Begin*/
		for(int i = 0; i <= bit_pointer; i++)
		{
			bit_array[i] = 0;
		}
		if(Range_w)
			lcdRectClear(126, 425, 198, 437, BACKGROUND);
		else
			lcdRectClear(126, 385, 198, 397, BACKGROUND);
		bit_pointer = 0;
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y+150, Num_Y+200, page) == 0)
	{
		lcddrawsqur(Num_X+100, Num_X+150, Num_Y+150, Num_Y+200, BLACK, " clc");
	}

	/***************** -mode- ********************/
	if(touchResponse(Num_X, Num_X+50, Num_Y+210, Num_Y+250, page)==1)
	{
		/*�������ܶ���������Begin*/
		if(mode == 0){
			mode = 1;
			draw_flag = 1;
			Troubleshooting = 0;
			lcdRectClear(60, 335, 190, 365, BACKGROUND);
			lcdDispStringSmall(60, 344, RED, WHITE, "Drawing");

		}
		else if(mode == 1){
			mode = 2;
			Troubleshooting = 1;
			draw_flag = 0;
			lcdRectClear(60, 335, 190, 365, BACKGROUND);
			lcdDispStringSmall(60, 344, RED, WHITE, "Error diagnosis");
		}
		else{
			mode = 0;
			draw_flag = 0;
			Troubleshooting = 0;
			lcdRectClear(60, 335, 190, 365, BACKGROUND);
			lcdDispStringSmall(60, 344, RED, WHITE, "Measuring");
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X, Num_X+50, Num_Y+210, Num_Y+250, page) == 0)
	{
		lcddrawsqur(Num_X, Num_X+50, Num_Y+210, Num_Y+250, BLACK, "Mode");
	}

	/***************** -F_L- *********************/
	if(touchResponse(Num_X+50, Num_X+100, Num_Y+210, Num_Y+250, page)==1)
	{
		/*�������ܶ���������Begin*/
		if(Range_w == 1){
			Range_w = 0;//�޸�F_L��ֵ
			for(int i = 0; i <= bit_pointer; i++)
			{
				bit_array[i] = 0;
			}
			bit_pointer = 0;
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+50, Num_X+100, Num_Y+210, Num_Y+250, page) == 0)
	{
		lcddrawsqur(Num_X+50, Num_X+100, Num_Y+210, Num_Y+250, BLACK, "F_L");
	}

	/***************** -F_H- *********************/
	if(touchResponse(Num_X+100, Num_X+150, Num_Y+210, Num_Y+250, page)==1)
	{
		/*�������ܶ���������Begin*/
		if(!Range_w){
			Range_w = 1;//�޸�F_H
			for(int i = 0; i <= bit_pointer; i++)
			{
				bit_array[i] = 0;
			}
			bit_pointer = 0;
		}
		/*�������ܶ���������End*/
	}
	else if(touchResponse(Num_X+100, Num_X+150, Num_Y+210, Num_Y+250, page) == 0)
	{
		lcddrawsqur(Num_X+100, Num_X+150, Num_Y+210, Num_Y+250, BLACK, " F_H");
	}
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

