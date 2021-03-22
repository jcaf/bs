#include <Arduino.h>
#include "getString.h"
#include "system.h"
#include "sysprint.h"
#include "lcd_fxaux.h"

void getString_voltage(float V, char *str_dest)
{
    char char_buff[DTOSTRF_SIZE_BUF];


    strcpy_P(str_dest, PSTR("V="));
    strcat(str_dest, dtostrf(V, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) );
    strcat_P(str_dest, PSTR(" V"));
}
void getString_current(float I, char *str_dest)
{
    char char_buff[DTOSTRF_SIZE_BUF];

    strcpy_P(str_dest, PSTR("I="));
    strcat(str_dest, dtostrf(I, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) );
    strcat_P(str_dest, PSTR(" A"));
}
void getString_temperature(float T, char *str_dest)
{
    char char_buff[DTOSTRF_SIZE_BUF+10];

    strcpy_P(str_dest, PSTR("T="));
    strcat(str_dest, dtostrf(T, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) );
    strcat_P(str_dest, PSTR(" C"));
}

void getString_error(const char *Text, uint8_t E, char *str_dest)
{

    char buff[LCDAN_STR_MAX_SIZE];

    strcpy(str_dest, Text);
    itoa(E, buff, 10);
    strcat(str_dest, buff);
}

void getString_cell_voltage(float CV, uint8_t num_cell, char *str_dest)
{


    char char_buff[DTOSTRF_SIZE_BUF];
    char buff[LCDAN_STR_MAX_SIZE];

    strcpy_P(str_dest, PSTR("C"));
    itoa(num_cell, buff, 10);
    strcat(str_dest, buff);
    strcat_P(str_dest, PSTR("="));
    strcat(str_dest, dtostrf(CV, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff));
    strcat_P(str_dest, PSTR(" V"));
}


