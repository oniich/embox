/**
 * @file spi.h
 * @brief Interface for SPI devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 14.06.2017
 */

#ifndef DRIVER_SPI_CORE_H_
#define DRIVER_SPI_CORE_H_

enum spi_mode {
	SPI_MODE_MASTER,
	SPI_MODE_SLAVE,
};

struct spi_slave {
	int bus;
	int cs;
	enum spi_mode mode;
};

struct spi_ops {
	uint8_t (*transfer_byte)(struct
};

struct spi_slave *spi_slave_connect(int bus, int cs);

#endif /* DRIVER_SPI_CORE_H_ */
