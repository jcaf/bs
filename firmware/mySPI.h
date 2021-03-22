#ifndef MYSPI_H_
#define MYSPI_H_

#include <stdint.h>


void spi_write_array(uint8_t len, uint8_t *data);
void spi_write_read(uint8_t *tx_Data, uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len );
void spi_write(uint8_t  data);
uint8_t spi_read(uint8_t  data);

void SPI_deselectAllperipherals(void);

#endif // MYSPI_H_
