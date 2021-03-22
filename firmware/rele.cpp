#include <Arduino.h>
#include "rele.h"
#include "system.h"

uint8_t do_rele[NUM_RELE_MAX];

uint8_t rele_1_readvalue(void)
{
    return ReadPin(PORTRxRELE1, PINxRELE1);
}
uint8_t rele_2_readvalue(void)
{
    return ReadPin(PORTRxRELE2, PINxRELE2);
}
uint8_t rele_3_readvalue(void)
{
    return ReadPin(PORTRxRELE3, PINxRELE3);
}
uint8_t rele_4_readvalue(void)
{
    return ReadPin(PORTRxRELE4, PINxRELE4);
}
uint8_t rele_5_readvalue(void)
{
    return ReadPin(PORTRxRELE5, PINxRELE5);
}

/////////////////////////////////////////////////////////
//const struct _do_rele_readPin
//{
//    PTRFX_retUINT16_T read;
//
//} do_rele_readPin[NUM_RELE_MAX] PROGMEM=
//{
//    rele_1_readvalue,
//    rele_2_readvalue,
//    rele_3_readvalue,
//    rele_4_readvalue,
//    rele_5_readvalue,
//};

/////////////////////////////////////////////////////////
void rele_1_outvalue(uint8_t val)
{
     if (val == 1)
        PinTo1(PORTWxRELE1, PINxRELE1);
     else
        PinTo0(PORTWxRELE1, PINxRELE1);
}
void rele_2_outvalue(uint8_t val)
{
    if (val == 1)
        PinTo1(PORTWxRELE2, PINxRELE2);
    else
        PinTo0(PORTWxRELE2, PINxRELE2);
}
void rele_3_outvalue(uint8_t val)
{
    if (val == 1)
        PinTo1(PORTWxRELE3, PINxRELE3);
    else
        PinTo0(PORTWxRELE3, PINxRELE3);
}
void rele_4_outvalue(uint8_t val)
{
    if (val == 1)
        PinTo1(PORTWxRELE4, PINxRELE4);
    else
        PinTo0(PORTWxRELE4, PINxRELE4);
}
void rele_5_outvalue(uint8_t val)
{
    if (val == 1)
        PinTo1(PORTWxRELE5, PINxRELE5);
    else
        PinTo0(PORTWxRELE5, PINxRELE5);
}

/*
struct _doRele
{
	PTRFX_retUINT8_T read;

}doRele[NUM_RELE_MAX]=
{
	do_rele_readRELE1,
	do_rele_readRELE2,
	do_rele_readRELE3,
	do_rele_readRELE4,
	do_rele_readRELE5,
};

void do_rele_read(void)
{
	for (uint8_t i=0; i<NUM_RELE_MAX; i++ )
		do_rele[i] = doRele[i].read();
}
*/


void rele_job(void)
{
//	for (uint8_t i=0; i<NUM_RELE_MAX; i++ )
//    {
//        do_rele[i] = doRele[i].read();
//    }

    do_rele[0] = ReadPin(PORTRxRELE1, PINxRELE1);
    do_rele[1] = ReadPin(PORTRxRELE2, PINxRELE2);
    do_rele[2] = ReadPin(PORTRxRELE3, PINxRELE3);
    do_rele[3] = ReadPin(PORTRxRELE4, PINxRELE4);
    do_rele[4] = ReadPin(PORTRxRELE5, PINxRELE5);

}

void rele_init(void)
{
    PinTo0(PORTRxRELE1, PINxRELE1);
    PinTo0(PORTRxRELE2, PINxRELE2);
    PinTo0(PORTRxRELE3, PINxRELE3);
    PinTo0(PORTRxRELE4, PINxRELE4);
    PinTo0(PORTRxRELE5, PINxRELE5);

    ConfigOutputPin(DDRxRELE1, PINxRELE1);
    ConfigOutputPin(DDRxRELE2, PINxRELE2);
    ConfigOutputPin(DDRxRELE3, PINxRELE3);
    ConfigOutputPin(DDRxRELE4, PINxRELE4);
    ConfigOutputPin(DDRxRELE5, PINxRELE5);
}




