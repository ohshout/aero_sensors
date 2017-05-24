/**
 ******************************************************************************
 * @addtogroup PIOS PIOS Core hardware abstraction layer
 * @{
 * @addtogroup PIOS_I2C I2C Functions
 * @brief      Linux I2C functionality
 * @{
 *
 * @file       pios_i2c.c
 * @author     dRonin, http://dRonin.org/, Copyright (C) 2017
 *
 * @brief      I2C routines
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 */

/* Project Includes */
//#include <pios.h>
//#include <pios_mutex.h>
#include "i2c.h"
#include "i2c_priv.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

//#define DEBUG_I2C

int32_t PIOS_I2C_Init(const char *path)
{
	int fd = open(path, O_RDWR);

	if (fd < 0) {
		perror("i2c-open");
		return -2;
	}

	/* No error */
	return fd;
}

static void print_i2c_msg(struct i2c_msg * msg)
{
	int j;

	printf("buf: [");
	for (j = 0; j < msg->len; j++)
		printf("0x%X ", msg->buf[j]);
	printf("]. len=%d\n", msg->len);

}

int32_t PIOS_I2C_Transfer(int32_t fd, const struct pios_i2c_txn txn_list[], uint32_t num_txns)
{
	int ret = 0, i;

	if (num_txns > 10) {
		return -1;
	}

	struct i2c_msg msgs[num_txns];

	for (i = 0; i < num_txns; i++) {
		msgs[i].addr = txn_list[i].addr;
		if (txn_list[i].rw == PIOS_I2C_TXN_WRITE) {
			msgs[i].flags = 0;
		} else {
			msgs[i].flags = I2C_M_RD;
		}
		msgs[i].buf = txn_list[i].buf;
		msgs[i].len = txn_list[i].len;
	}

	struct i2c_rdwr_ioctl_data msgset = {
		.msgs = msgs,
		.nmsgs = num_txns
	};

#ifdef DEBUG_I2C
	printf("\n-------MSG-------\n");
	/* print all WRITE msgs */
	printf("TX: ");
	int num = msgset.nmsgs;
	for (i = 0; i < num; i++) {
		if (msgset.msgs[i].flags != I2C_M_RD) {
			print_i2c_msg(&(msgset.msgs[i]));
		}
	}
#endif

	if (ioctl(fd, I2C_RDWR, &msgset) < 0) {
		perror("i2c-ioctl"); /* XXX remove */
		ret = -1;
	}

#ifdef DEBUG_I2C
	/* print all READ msgs */
	printf("RX: ");
	for (i = 0; i < num; i++) {
		if (msgset.msgs[i].flags == I2C_M_RD) {
			print_i2c_msg(&(msgset.msgs[i]));
		}
	}
#endif

	return ret;
}

/**
  * @}
  * @}
  */
