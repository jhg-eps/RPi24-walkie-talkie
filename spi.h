#ifndef SPI_H
#define	SPI_H
#include <stdint.h>
#include <stdio.h>      
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <inttypes.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include "gpio.h"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
#include <bcm2835.h>

typedef struct spi_state SPIState; /* opaque type definition */

SPIState * spi_init();
void spi_enable(SPIState *spi); 
uint8_t spi_transfer(uint8_t val, uint8_t *rx);
//uint8_t spi_transfer_bulk(SPIState *spi, uint8_t *tx, uint8_t *rx, uint8_t len);
void spi_disable(SPIState *spi);
void spi_close(SPIState *spi);

#endif	/* SPI_H */
