#include "spi.h"
//#include "RF24/utility/RPi/bcm2835.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define BUF_LEN 1
#define SET_SPI(_spi, _mode, _bits, _speed, _chip_select) do {\
	_spi->mode = _mode;\
	_spi->bits = _bits;\
	_spi->speed = _speed;\
	_spi->chip_select = _chip_select;\
} while (0)

typedef struct spi_state {
	uint32_t speed;
	uint32_t mode;
	uint8_t bits;
	int fd;
	uint8_t chip_select;
	pthread_mutex_t lock;
} SPIState;

SPIState * spi_init() {
	seteuid(0); // make sure euid is set to root's UID
	//printf("root was successful: %d", setToRoot); // 0 if successful
	if (!bcm2835_init()) // Initialize the BCM2835 SPI library
		perror("Unable to initialize the BCM2835 library");
	bcm2835_spi_begin();
	//bcm2835_spi_end();
	//if (!bcm2835_spi_begin())  // Set the SPI bus & related pins to SPI exclusivity
	//	perror("Unable to set the GPIO pins for SPI access");
		//printf("more 1");
	bcm2835_spi_setDataMode(0);  // CPOL = 1, CPHA = 0
	bcm2835_spi_setClockDivider(64); // clock => clock / 2^(6)
	bcm2835_spi_chipSelect(0);
	bcm2835_spi_setChipSelectPolarity(0, 0); // Set CE0 (first 0) to active low (second zero)
	//printf("more 2");
	
	SPIState *spi = (SPIState *) malloc(sizeof(SPIState));
	if (spi == NULL) return NULL;
	//SET_SPI(spi, mode, bits, speed, chip_select);
	//printf("SPI config: mode %d, %d bit, %dMhz\n",spi->mode, spi->bits, (spi->speed)/1000000);
	//spi->fd = open(device, O_RDWR);

//	gpio_open(spi->chip_select, GPIO_OUT);
	spi_disable(spi); /* Ensures chip select is pulled up */
	pthread_mutex_init(&(spi->lock), NULL);
	return spi;
}

void spi_enable(SPIState *spi){
	pthread_mutex_lock(&(spi->lock));
	#if 0
	gpio_write(spi->chip_select, GPIO_LOW);
	#endif
}

 uint8_t spi_transfer(uint8_t val, uint8_t *rx) {
	uint8_t res = 0;
	if (rx != NULL)
		*rx = bcm2835_spi_transfer(val); // transfer a byte through SPI and read the returned byte
	else 
		res = bcm2835_spi_transfer(val);
	return res;
	//return 1; // preserve return 1 for stability
}

//uint8_t spi_transfer_bulk(SPIState *spi, uint8_t *tx, uint8_t *rx, uint8_t len) {
//	// dead CODE!!!
//	bcm2835_spi_transfernb( tbuf, rbuf, len)
//	return 1; // also preserved for stability
//}

void spi_disable(SPIState *spi){
	#if 0 
	gpio_write(spi->chip_select, GPIO_HIGH);
	#endif
	pthread_mutex_unlock(&(spi->lock));
}

void spi_close(SPIState *spi){
	// free(spi);??????
	pthread_mutex_lock(&(spi->lock));
	#if 0
	close(spi->fd);
	#endif
	bcm2835_spi_end(); // free the SPI bus & related pins to normal GPIO usage 
	bcm2835_close(); // deallocates any memory, closing /dev/mem or /dev/gpiomem
	pthread_mutex_unlock(&(spi->lock));
}

