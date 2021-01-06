/*
 * touch.c
 *
 *  Created on: May 23, 2017
 *      Author: hailiang
 */

#include "touch.h"
#include "lcd.h"

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

	//��������
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

