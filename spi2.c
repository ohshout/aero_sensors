/* SPI Transfer basic application
 *
 *  Copyright (c) 2016 Intel Corporation
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Author: Israel Cepeda <israel.a.cepeda.lopez@intel.com>
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPIDEV "/dev/spidev%d.%d"

struct spidev {
	char node[20];
	uint8_t mode;
	uint8_t bits;
	uint32_t speed;
	int fd;
};

struct transfer {
	int words;
	long data;
	int len;
};

/*
 * Transfer function
 */
static int transfer(struct spidev *dev, const uint8_t * tx, uint8_t * rx, uint16_t len)
{
	struct spi_ioc_transfer iotr;
	//uint8_t *tx, *rx;
	int ret;//, len = dev->bits/8 * tr->words;
	int i;

	//if (len < tr->len)
	//	len = tr->len;

	//tx = malloc(len);
	//rx = malloc(len);

	//if (!tx || !rx) {
	//	printf("ERROR: No memory\n");
	//	return;
	//}

	memset(&iotr, 0, sizeof(iotr));
	//memcpy(tx, &tr->data, tr->len);
	//memset(&tx[tr->len], 0x00, len - tr->len);
	//memset(rx, 0x00, len);

#ifdef SPI_DEBUG
	printf("TX: ");
	for (i = 0; i < len; i++)
		printf("0x%x ", tx[i]);
	printf("\n");
#endif

	/* prepare transfer */
	iotr.tx_buf = (unsigned long)tx;
	iotr.rx_buf = (unsigned long)rx;
	iotr.len = len;

	ret = ioctl(dev->fd, SPI_IOC_MESSAGE(1), &iotr);
	if (ret < 1)
		printf("ERROR: failed to transfer %s\n", dev->node);
	else {
#ifdef SPI_DEBUG
		printf("RX: ");
		for (i = 0; i < len; i++)
			printf("0x%x ", rx[i]);
		printf("\n");
#endif
	}

	//free(tx);
	//free(rx);
	return ret;
}

int32_t PIOS_SPI_TransferBlock(struct spidev * spi_dev, const uint8_t *send_buffer, uint8_t *receive_buffer, uint16_t len)
{
	return transfer(spi_dev, send_buffer, receive_buffer, len);
}

//uint8_t PIOS_SPI_TransferByte(struct spidev * spi_dev, uint8_t b)
//{
//	uint8_t ret = 0;
//
//	PIOS_SPI_TransferBlock(spi_dev, &b, &ret, 1);
//
//	return ret;
//}

/*
 * Main function
 */
int SPI_Init(struct spidev * dev)
{
	//struct spidev dev;
	//struct transfer tr;
	int bus = 0, cs = 0, words = 0, ret = 0;
	char c, *data = 0;

#if 0
	while ((c = getopt(argc, argv, "b:c:w:d:")) != -1) {
		switch (c) {
		case 'b':
			bus = atoi(optarg);
			break;
		case 'c':
			cs = atoi(optarg);
			break;
		case 'd':
			data = optarg;
			break;
		case 'w':
			words = atoi(optarg);
			break;
		default:
			return -1;
		}
	}
#endif

	/* /dev/spidev3.0 */
	bus = 3; cs = 0;
	sprintf(dev->node, SPIDEV, bus, cs);

	/* open spi device */
	dev->fd = open(dev->node, O_RDWR);
	if (dev->fd < 0) {
		printf("ERROR: failed to open %s\n", dev->node);
		ret = -errno;
		return ret;
	}

	/* spi mode */
	ret = ioctl(dev->fd, SPI_IOC_RD_MODE, &dev->mode);
	if (ret == -1)
		printf("ERROR: faild to get mode %s\n", dev->node);

	/* bits per word */
	ret = ioctl(dev->fd, SPI_IOC_RD_BITS_PER_WORD, &dev->bits);
	if (ret == -1)
		printf("ERROR: faild to get bits per word %s\n", dev->node);

	/* max speed hz */
	ret = ioctl(dev->fd, SPI_IOC_RD_MAX_SPEED_HZ, &dev->speed);
	if (ret == -1)
		printf("ERROR: faild to get max speed hz %s\n", dev->node);

	/* spi device info */
	printf("spi mode: %d\n", dev->mode);
	printf("bits per word: %d\n", dev->bits);
	printf("max speed: %d Hz\n", dev->speed);

#if 0
	if (data) {
		tr.words = words > 0 ? words : 1;
		tr.data = strtol(data, NULL, 16);
		tr.len = strlen(data) / (2 * dev.bits / 8);
		transfer(&dev, &tr);
	} else
		printf("ERROR: No data specified\n");
#endif

	/* close spi device */
	//close(dev.fd);

	return ret;
}

int SPI_DeInit(struct spidev * dev)
{
	close (dev->fd);
}
