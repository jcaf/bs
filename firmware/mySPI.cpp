#include <Arduino.h>
#include "system.h"
#include "LTC68042.h"
#include "mySPI.h"

void spi_write_array(uint8_t len, uint8_t *data)
{
    for(uint8_t i = 0; i < len; i++)
        spi_write(data[i]);
 }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t *tx_Data,//array of data to be written on SPI port
//uint8_t tx_len, //length of the tx data array
//uint8_t *rx_data,//Input: array that will store the data read by the SPI port
//uint8_t rx_len //Option: number of bytes to be read from the SPI port
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void spi_write_read(uint8_t *tx_Data, uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len )
{
    for(uint8_t i = 0; i < tx_len; i++)
        spi_write(tx_Data[i]);

    for(uint8_t i = 0; i < rx_len; i++)
        rx_data[i] = (uint8_t)spi_read(0xFF);
}
void spi_write(uint8_t  data)
{
    SPDR = data;
    while(!(SPSR & (1<<SPIF))) ;
    uint8_t dummy = SPDR;
}

uint8_t spi_read(uint8_t  data)
{
    SPDR = data;
    while(!(SPSR & (1<<SPIF))) ;
    return SPDR;
}



void SPI_deselectAllperipherals(void)
{
    digitalWrite(WIZNET_CS, HIGH);
    digitalWrite(SDCARD_CS, HIGH);
    digitalWrite(LTC6804_CS, HIGH);
}
