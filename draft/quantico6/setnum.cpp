/****************************************************************************
* Project: Battery System
*
* 2016-05-24-16.04
* jcaf @ jcafOpenSuse
*
* Copyright 2013 Juan Carlos AgÃ¼ero Flores
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
****************************************************************************/
#include <Arduino.h>
#include "system.h"
#include "utils.h"
#include "setnum.h"

struct _setnum setnum;

void setnum_digit_set_position(int8_t position)
{
    setnum.digit.position = position;
}
int8_t setnum_digit_get_position(void)
{
    return setnum.digit.position;
}
void setnum_digit_blink_set_timeout(unsigned long timeout)
{
    setnum.digit.blink.timeout = timeout;
}
void setnum_charnumber_qty_of_charnumber(uint8_t qty_of_charnumber)
{
    setnum.charnumber.qty_of_charnumber = qty_of_charnumber;
}
void setnum_charnumber_thenumber_digitposition_set_dp(uint8_t selected, uint8_t digitposition_decimalpoint)
{
    setnum.charnumber.thenumber[selected].digitposition.dp = digitposition_decimalpoint;
}
void setnum_charnumber_thenumber_digitposition_set_endofdigit(uint8_t selected, uint8_t digitposition_endofdigit)
{
    setnum.charnumber.thenumber[selected].digitposition.endofdigit = digitposition_endofdigit;
}
void setnum_printxy_register_pFil(uint8_t (*pFil) (uint8_t))
{
    setnum.printxy.pFil = pFil;
}
void setnum_printxy_register_pCol(uint8_t (*pCol) (uint8_t))
{
    setnum.printxy.pCol = pCol;
}
void setnum_charnumber_thenumber_digitposition_init(uint8_t selected, uint8_t digitposition_decimalpoint, uint8_t digitposition_endofdigit)
{
    setnum_charnumber_thenumber_digitposition_set_dp(selected,digitposition_decimalpoint);
    setnum_charnumber_thenumber_digitposition_set_endofdigit(selected, digitposition_endofdigit);
}

void setnum_charnumber_thenumber_digitDec(void)//up
{
    if (--setnum.charnumber.thenumber[setnum.charnumber.selected].charvalue[setnum.digit.position]< '0')
        setnum.charnumber.thenumber[setnum.charnumber.selected].charvalue[setnum.digit.position] = '9';

    setnum_digit_blink_set2noblank();//force view the digit
}

void setnum_charnumber_thenumber_digitInc(void)//down
{
    if (++setnum.charnumber.thenumber[setnum.charnumber.selected].charvalue[setnum.digit.position] > '9')
        setnum.charnumber.thenumber[setnum.charnumber.selected].charvalue[setnum.digit.position] = '0';

    setnum_digit_blink_set2noblank();//fix the digit
}
void setnum_charnumber_selectDigit_selectNumber(void)//enter
{
    char c = setnum.charnumber.thenumber[setnum.charnumber.selected].charvalue[setnum.digit.position];
    setnum_digit_blink_print_digit(c);//fix current digit before to change to right


    setnum.digit.position++;

    if (setnum.charnumber.thenumber[setnum.charnumber.selected].digitposition.dp > 0)
    {
        if (setnum.digit.position == setnum.charnumber.thenumber[setnum.charnumber.selected].digitposition.dp)
            setnum.digit.position++;//skip decimal point
    }

    if (setnum.digit.position > setnum.charnumber.thenumber[setnum.charnumber.selected].digitposition.endofdigit)//limit ESTE DEBE SER MENOR EN 1
    {
//        char c = setnum.charnumber.thenumber[setnum.charnumber.selected].charvalue[--setnum.digit.position];
//        setnum_digit_blink_print_digit(c);//fix latest digit before to leave
//        //
        setnum.digit.position = 0;

        if (++setnum.charnumber.selected == setnum.charnumber.qty_of_charnumber)
            setnum.charnumber.selected = 0x00;
    }
    setnum_digit_blink_set2blank();//Enter with digit in "blank"
}
void setnum_digit_blink_set2blank(void)
{
    setnum.digit.blink.sm0 = 1;
    setnum.digit.blink.bf.toggle = BLINK_TOGGLE_BLANK;
}

void setnum_digit_blink_set2noblank(void)
{
    setnum.digit.blink.sm0 = 1;
    setnum.digit.blink.bf.toggle = BLINK_TOGGLE_NOBLANK;
}

static inline char setnum_digit_blink_getDigit(uint8_t blink_toggle)
{
    if (blink_toggle == BLINK_TOGGLE_NOBLANK)
        return  setnum.charnumber.thenumber[setnum.charnumber.selected].charvalue[setnum.digit.position];
    else
        return ' ';
}

void setnum_digit_blinking(void)
{
    if (setnum.digit.blink.sm0 == 0)
    {
        if ( (millis() - setnum.digit.blink.last_millis) > setnum.digit.blink.timeout)
        {
            setnum.digit.blink.bf.toggle = !setnum.digit.blink.bf.toggle;
            setnum.digit.blink.sm0++;
        }
    }
    if (setnum.digit.blink.sm0 == 1)
    {
        char c = setnum_digit_blink_getDigit(setnum.digit.blink.bf.toggle);//turno x blank or digit
        setnum_digit_blink_print_digit(c);
        //
        setnum.digit.blink.last_millis = millis();
        setnum.digit.blink.sm0 = 0;
    }
}

void setnum_digit_blink_print_digit(char c)
{
    uint8_t fil = setnum.printxy.pFil(setnum.charnumber.selected);
    uint8_t col = setnum.printxy.pCol(setnum.charnumber.selected) + setnum.digit.position;
    lcd.printAtPosc(fil, col, String(c));
}
//
void setnum_charnumber_print_charvalue(uint8_t selected)
{
    uint8_t fil = setnum.printxy.pFil(selected);
    uint8_t col = setnum.printxy.pCol(selected);
    lcd.printAtPosc(fil,col, String(setnum.charnumber.thenumber[selected].charvalue));
}
void setnum_charnumber_print_charvalue_fCurrSelected(void)
{
    setnum_charnumber_print_charvalue(setnum.charnumber.selected);
}
//
void setnum_charnumber_set_charvalue_asDou_aftFillZeros(uint8_t selected, double dnumber, signed char __witdh, unsigned char __prec)
{
    dtostrf(dnumber, __witdh, __prec,  setnum.charnumber.thenumber[selected].charvalue);
    dtostrf_fill_zero(setnum.charnumber.thenumber[selected].charvalue);
}
void setnum_charnumber_set_charvalue_asDou_aftFillZeros_fcurrSelected(double dnumber, signed char __witdh, unsigned char __prec)
{
    setnum_charnumber_set_charvalue_asDou_aftFillZeros(setnum.charnumber.selected, dnumber, __witdh, __prec);
}
//
void setnum_charnumber_set_charvalue_asDou(uint8_t selected, double dnumber, signed char __witdh, unsigned char __prec)
{
    dtostrf(dnumber, __witdh, __prec,  setnum.charnumber.thenumber[selected].charvalue);
}
void setnum_charnumber_set_charvalue_asDou_fcurrSelected(double dnumber, signed char __witdh, unsigned char __prec)
{
    setnum_charnumber_set_charvalue_asDou(setnum.charnumber.selected, dnumber, __witdh, __prec);
}
//
//
double setnum_charnumber_get_charvalue_asDou(uint8_t selected)
{
    return strtod(setnum.charnumber.thenumber[selected].charvalue, (char**)0);
}
double setnum_charnumber_get_charvalue_asDou_fcurrSelected(void)
{
    return setnum_charnumber_get_charvalue_asDou(setnum.charnumber.selected);
}

uint8_t setnum_charnumber_get_currSelected(void)
{
    return setnum.charnumber.selected;
}

void setnum_charnumber_set_currSelected(uint8_t selected)
{
    setnum.charnumber.selected = selected;
}

void setnum_digit_reset_charnumber_reset(void)
{
    setnum_digit_set_position(0);
    setnum_digit_blink_set2noblank();
    //
    setnum_charnumber_set_currSelected(0);
}
