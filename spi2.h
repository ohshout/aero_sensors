#ifndef PIOS_SPI_H
#define PIOS_SPI_H

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

int SPI_Init(struct spidev *);

/**
 * (Re-)initialises SPI peripheral clock rate
 *
 * \param[in] spi SPI number (0 or 1)
 * \param[in] spi_speed configures the SPI speed in Hz
 * \return The actual attained/configured speed.
 */
//int32_t PIOS_SPI_SetClockSpeed(uint32_t spi_id, uint32_t speed);
int32_t PIOS_SPI_SetClockSpeed(int spi_fd, uint32_t speed);

/**
* Controls the slave select on a SPI port.
* \param[in] spi_id the SPI handle
* \param[in] slave_id the index of the slave to select/deselect
* \param[in] pin_value true to deselect slave, false to select
* \return 0 if no error
*/
int32_t PIOS_SPI_RC_PinSet(uint32_t spi_id, uint32_t slave_id, bool pin_value);

/**
* Transfers a byte to SPI output and reads back the return value from SPI input
* \param[in] spi_id SPI handle
* \param[in] b the byte which should be transfered out
* \return The received byte
*/
//uint8_t PIOS_SPI_TransferByte(uint32_t spi_id, uint8_t b);
uint8_t PIOS_SPI_TransferByte(struct spidev *, uint8_t b);

/**
* Transfers a block of bytes
* \param[in] spi_id SPI device handle
* \param[in] send_buffer pointer to buffer which should be sent.<BR>
* If NULL, 0xff (all-one) will be sent.
* \param[in] receive_buffer pointer to buffer which should get the received values.<BR>
* If NULL, received bytes will be discarded.
* \param[in] len number of bytes which should be transfered
* \return >= 0 if no error during transfer
*/
int32_t PIOS_SPI_TransferBlock(struct spidev *, const uint8_t *send_buffer, uint8_t *receive_buffer, uint16_t len);

/**
 * Claim the SPI bus semaphore.
 * \param[in] spi_id SPI handle
 * \return 0 if no error
 * \return -1 if timeout before claiming semaphore
 */
int32_t PIOS_SPI_ClaimBus(uint32_t spi_id);

/**
 * Release the SPI bus semaphore.
 * \param[in] spi_id SPI handle
 * \return 0 if no error
 */
int32_t PIOS_SPI_ReleaseBus(uint32_t spi_id);

void    PIOS_SPI_IRQ_Handler(uint32_t spi_id);

#endif /* PIOS_SPI_H */

/**
 * @}
 * @}
 */
