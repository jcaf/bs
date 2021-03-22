#ifndef RELE_H_
#define RELE_H_

#define NUM_RELE_MAX 5

#define RELE1 29
#define RELE2 30
#define RELE3 31
#define RELE4 32
#define RELE5 33


//ATmega2560 pinout
#define DDRxRELE1	DDRA
#define PORTWxRELE1	PORTA
#define PORTRxRELE1	PINA
#define PINxRELE1	7

#define DDRxRELE2	DDRC
#define PORTWxRELE2	PORTC
#define PORTRxRELE2	PINC
#define PINxRELE2	7

#define DDRxRELE3	DDRC
#define PORTWxRELE3	PORTC
#define PORTRxRELE3	PINC
#define PINxRELE3	6

#define DDRxRELE4	DDRC
#define PORTWxRELE4	PORTC
#define PORTRxRELE4	PINC
#define PINxRELE4	5

#define DDRxRELE5	DDRC
#define PORTWxRELE5	PORTC
#define PORTRxRELE5 PINC
#define PINxRELE5	4



extern uint8_t do_rele[NUM_RELE_MAX];
void rele_job(void);
void rele_init(void);
#endif


