/*
 * GccApplication1.c
 *
 * Created: 26/11/2015 08:04:33 PM
 * Author : jcaf
 */ 

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
//TEST isoSPI LTC6820
void SPI_MasterInit(void)
{
  DDRB = (1<<PB4) | (1<<PB2)|(1<<PB1) | (1<<PB0);
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0) ;//|(1<<SPR1);
}
char SPI_MasterTransmit(char cData)
{
  
  SPDR = cData;
  
  while ( !(SPSR & (1<<SPIF)) )  {;}
	//  _delay_ms(10);
	return SPDR;
}

int main(void)
{
	int i;
	DDRE=1<<PE3;//PIN 5 DEL ARDUINO
	
	DDRG = 1<<PG5;
	PORTG = 1<<PG5;
	
	SPI_MasterInit();
	PORTB=(1<<4) | (1<<0);
    
	_delay_ms(500);
	PORTE = 0<<PE3;
	while (1) 
    {
		/*
		PORTB=0xFF;
		_delay_ms(100);
		PORTB=0x0;
		_delay_ms(100);
		*/
		//PORTE = 0<<PE3;
		//_delay_ms(1);
			for (i=0; i<100; i++)
			{
			SPI_MasterTransmit(0xAA);
			}
			
		//	_delay_ms(10);
		//PORTE = 1<<PE3;
		//_delay_ms(100);
    }
}

