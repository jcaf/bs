/****************************************************************************
* Project: Battery System
*
* 2016-10-07-12.18
* jcaf @ jcafOpenSuse
*
* Copyright 2013 Juan Carlos Agüero Flores
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
#include "logf.h"
#include "logf_class.h"

#include "digital_input.h"
#include "analog_input.h"
#include "rele.h"
#include <EEPROM.h>
#include "myEEPROM.h"

class _logf_digital_input logf_digital_input[NUM_DIG_INPUT_MAX];
class _logf_analog_input logf_analog_input[NUM_ANALOG_INPUT_MAX];

struct _logf_rele_bf logf_rele_bf[NUM_RELE_MAX];
class _logf_rele logf_rele[NUM_RELE_MAX];

class _Symbol Symbol;
class _logf_outAssign logf_outAssign;
struct _logf_logExp_assigOut_data logf_logExp_assigOut_data;
//
class _Symbol Symbol_d;
class _logf_outAssign logf_outAssign_d;
struct _logf_logExp_assigOut_data logf_logExp_assigOut_data_d;

uint8_t num_exp_saved;

void logf_init(void)
{
    logf_digital_input[0].set_logic(LOGF_DIGINPUT_POSITIVE_LOGIC);
    logf_digital_input[1].set_logic(LOGF_DIGINPUT_POSITIVE_LOGIC);
    logf_digital_input[2].set_logic(LOGF_DIGINPUT_POSITIVE_LOGIC);
    logf_digital_input[3].set_logic(LOGF_DIGINPUT_POSITIVE_LOGIC);

    logf_digital_input[0].set_addr_fx_read_pin(digital_input_readvalDI1);
    logf_digital_input[1].set_addr_fx_read_pin(digital_input_readvalDI2);
    logf_digital_input[2].set_addr_fx_read_pin(digital_input_readvalDI3);
    logf_digital_input[3].set_addr_fx_read_pin(digital_input_readvalDI4);

    //
    for (uint8_t i=0; i<NUM_ANALOG_INPUT_MAX; i++)
    {
        EEPROM.get(EEPROM_ADDR_ANINPUT_FACTORS_DATA + ( sizeof(struct _an_input_factors) * i ) , logf_analog_input[i].an_input_factors );
    }
//    logf_analog_input[0].set_scale_factor(1);//*
//    logf_analog_input[0].set_offset(0);//+
//    logf_analog_input[0].set_units(LOGF_ANINPUT_UNITS_ADIMENSIONAL);
//    logf_analog_input[1].set_scale_factor(1);//*
//    logf_analog_input[1].set_offset(0);//+
//    logf_analog_input[1].set_units(LOGF_ANINPUT_UNITS_ADIMENSIONAL);
//    logf_analog_input[2].set_scale_factor(1);//*
//    logf_analog_input[2].set_offset(0);//+
//    logf_analog_input[2].set_units(LOGF_ANINPUT_UNITS_ADIMENSIONAL);
//    logf_analog_input[3].set_scale_factor(1);//*
//    logf_analog_input[3].set_offset(0);//+
//    logf_analog_input[3].set_units(LOGF_ANINPUT_UNITS_ADIMENSIONAL);

    logf_analog_input[0].set_addr_fx_read_pin(analog_input_readvalA1);
    logf_analog_input[1].set_addr_fx_read_pin(analog_input_readvalA2);
    logf_analog_input[2].set_addr_fx_read_pin(analog_input_readvalA3);
    logf_analog_input[3].set_addr_fx_read_pin(analog_input_readvalA4);


    for (uint8_t i=0; i<NUM_RELE_MAX; i++)
    {
        EEPROM.get(EEPROM_ADDR_RELE_BUSY + ( sizeof(struct _logf_rele_bf) * i ) , logf_rele[i].bf );
    }
//    logf_rele[0].bf.busy = 0;
//    logf_rele[1].bf.busy = 0;
//    logf_rele[2].bf.busy = 0;
//    logf_rele[3].bf.busy = 0;
//    logf_rele[4].bf.busy = 0;

    //override enable (no used)
    logf_rele[0].bf.enable = 1;
    logf_rele[1].bf.enable = 1;
    logf_rele[2].bf.enable = 1;
    logf_rele[3].bf.enable = 1;
    logf_rele[4].bf.enable = 1;


    logf_rele[0].set_addr_fx_write_pin(rele_1_outvalue);
    logf_rele[0].set_addr_fx_read_pin(rele_1_readvalue);
    logf_rele[1].set_addr_fx_write_pin(rele_2_outvalue);
    logf_rele[1].set_addr_fx_read_pin(rele_2_readvalue);
    logf_rele[2].set_addr_fx_write_pin(rele_3_outvalue);
    logf_rele[2].set_addr_fx_read_pin(rele_3_readvalue);
    logf_rele[3].set_addr_fx_write_pin(rele_4_outvalue);
    logf_rele[3].set_addr_fx_read_pin(rele_4_readvalue);
    logf_rele[4].set_addr_fx_write_pin(rele_5_outvalue);
    logf_rele[4].set_addr_fx_read_pin(rele_5_readvalue);
    //
    Symbol.set_data_addr(&logf_logExp_assigOut_data);//para crear nuevo exp
    logf_outAssign.set_data_addr(&logf_logExp_assigOut_data);//para crear nuevo exp
    //
    Symbol_d.set_data_addr(&logf_logExp_assigOut_data_d);//falta su otra pareja
    logf_outAssign_d.set_data_addr(&logf_logExp_assigOut_data_d);//para crear nuevo exp
    //
    EEPROM.get(EEPROM_ADDR_LOGF_EXP_NUM_EXP_SAVED, num_exp_saved);
}

////////////////////////////////////////////////
struct _logf_assigFxTimer
{
    uint8_t sm0;
    uint8_t valor;
    unsigned long last_millis;
}
logf_assigFxTimer[NUM_RELE_MAX];

////////////////////////////////////////////////
uint8_t logf_assig_fx_timer(uint8_t rele, uint8_t rele_activeif, uint8_t bool_val, float kte)
{
    uint8_t is_timming=0;
    if (logf_assigFxTimer[rele].sm0 == 0)//estable el valor al entrar
    {
        if (rele_activeif==1)
            logf_assigFxTimer[rele].valor = 0;  //para ambos casos si exp=1 or 0
        else
            logf_assigFxTimer[rele].valor = 1;   //para ambos casos si exp=1 or 0

        logf_rele[rele].set_value(logf_assigFxTimer[rele].valor);

        logf_assigFxTimer[rele].sm0++;
    }
    if (logf_assigFxTimer[rele].sm0 == 1)
    {
        if (bool_val==1)
        {
            logf_assigFxTimer[rele].last_millis = millis();//mantiene el valor e inicia la temporizacion No-interrupt
            //logf_assigFxTimer[rele].last_millis = 0x0000; //@interrupt
            logf_assigFxTimer[rele].sm0++;
        }
    }
    if (logf_assigFxTimer[rele].sm0 > 1)
    {
        if (bool_val == 0)
        {
            logf_assigFxTimer[rele].sm0 = 0x00;//resetea todo
        }
        else
        {
            if (logf_assigFxTimer[rele].sm0 == 2)
            {
                is_timming = 1;

                //if (++logf_assigFxTimer[rele].last_millis >=  (((unsigned long)kte)/5) )//cada 5ms @ i

                if ( (millis()-logf_assigFxTimer[rele].last_millis) >= (unsigned long)kte)
                {
                    logf_assigFxTimer[rele].valor = !logf_assigFxTimer[rele].valor;
                    logf_rele[rele].set_value(logf_assigFxTimer[rele].valor);
                    //
                    is_timming = 0;
                    logf_assigFxTimer[rele].sm0++;//espera al reset
                }
            }
        }
    }
    return is_timming;
}

void logf_d(void)
{
    class _logf_symbol result = Symbol_d.logExp_eval();
    uint8_t valui = result.get_valui();//si es una variable, este debe saber su valor BOOLEANO Y DE TIPO RESULT
    //logf_rele[0].set_value(valui);

    uint8_t length = logf_outAssign_d.get_length();//aqui esta el problema
    uint8_t rele;
    uint8_t rele_activeif;

    float valf;

    for (uint8_t i=0; i< length; i++)
    {
        rele = logf_outAssign_d.pdata->outAssign_arr[i].get_rele();
        rele_activeif = logf_outAssign_d.pdata->outAssign_arr[i].get_rele_activeif();

        if (logf_outAssign_d.pdata->outAssign_arr[i].get_type() == LOGF_ASSIG_TYPE_DIRECT_RELAY)
        {
            uint8_t bool_val;
            if (rele_activeif==0)
                bool_val = !valui;
            else
                bool_val = valui;

            logf_rele[rele].set_value(bool_val);
        }
        else if (logf_outAssign_d.pdata->outAssign_arr[i].get_type() == LOGF_ASSIG_TYPE_FX)
        {
            if (logf_outAssign_d.pdata->outAssign_arr[i].get_subtype() == LOGF_ASSIG_SUBTYPE_FX_TIMER)
            {
                float valf = logf_outAssign_d.pdata->outAssign_arr[i].get_valf();
                logf_assig_fx_timer(rele, rele_activeif, valui, valf);
            }
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
#include "myEEPROM.h"
void logf_logExpAssig_man(void)
{
    if (num_exp_saved > 0)//save in EEPROM
    {
        uint16_t EEaddr_logExp_assigOut_data = EEPROM_ADDR_LOGF_LOGEXP_ASSIGOUT_DATA;

        for (uint8_t i=0; i<num_exp_saved; i++)
        {
            EEPROM.get(EEaddr_logExp_assigOut_data, logf_logExp_assigOut_data_d);
            logf_d();
            EEaddr_logExp_assigOut_data  += (sizeof(struct _logf_logExp_assigOut_data));
        }
    }
}

