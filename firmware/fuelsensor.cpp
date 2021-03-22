/****************************************************************************
* Project: Battery System
*
* 2016-05-11-17.01
* jcaf @ jcafOpenSuse
*
* Copyright 2016 Juan Carlos AgÃ¼ero Flores
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
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"
#include "spline.h"
#include <EEPROM.h>
#include "rs485.h"
#include "sysprint.h"
#include "lcd_fxaux.h"


struct _fuelsensor fuelsensor;

void fuelsensor_init(void)
{
    fuelsensor_loaddata();
    sysPrint_serialPrintln("");
    sysPrint_serialPrintln_P(PSTR("fuelsensor: EEPROM was read"));
}

void fuelsensor_inhibit_comm(void)
{
    digitalWrite(RS485_DIR, RS485_TX);
    pinMode(RS485_DIR, OUTPUT);
}
void fuelsensor_set_comm(void)
{
    Serial2.begin(9600);//RS485 baud
    while (Serial2.available())
    {
        Serial2.read();
    }
}

void fuelsensor_check_consistency(void)
{
    if (fuelsensor.fsEE.tank.type == FS_TANK_TYPE_RECTANGULAR)
    {

        fuelsensor.fsEE.bf.enable_get_volume = fuelsensor.fsEE.bf.rectangular_area_consistent &&
                                               fuelsensor.fsEE.bf.lengthsTankdepth_consistent &&
                                               fuelsensor.fsEE.bf.full_calib && fuelsensor.fsEE.bf.zero_calib;

    }
    else if (fuelsensor.fsEE.tank.type == FS_TANK_TYPE_IRREGULAR)
    {
        fuelsensor.fsEE.bf.enable_get_volume = fuelsensor.fsEE.bf.irregular_spline_nodetable_consistent &&
                                               fuelsensor.fsEE.bf.lengthsTankdepth_consistent &&
                                               fuelsensor.fsEE.bf.full_calib && fuelsensor.fsEE.bf.zero_calib;

    }
    //-------------
    if (!fuelsensor.fsEE.bf.enable_get_volume)
    {
        fuelsensor.fsEE.outputType.type = (FS_OUTPUTTYPE)LENGTH;

        if (!fuelsensor.fsEE.bf.lengthsTankdepth_consistent)
        {
            fuelsensor.fsEE.outputType.length.units = FS_UNITMEA_LENGTH_PERCENTAGE;
        }
    }
    //fuelsensor_savedata();
}



/////////////////////////////////////////////////////////////////////////////////////////
//Can return:
// #define FS_COD_RET_FRAME_INWAITING  0
// #define FS_COD_RET_FRAME_CRC_BAD 3
// #define FS_COD_RET_FRAME_TIMEOUT 4
// #define FS_COD_RET_FRAME_ERROR 5
//#define FS_COD_RET_FRAME_OFFTIME_OR_LOST 6
//#define FS_COD_RET_FRAME_CYCLE_TXRX_OK 7

int8_t cod_ret_readfl=0;

void fuelsensor_D(void)
{
    static uint8_t sm0;

    if (sm0 == 0)
    {
        cod_ret_readfl = jt606_send_readFuelLevel();//internamente es non-blocking

        if (cod_ret_readfl != FS_COD_RET_FRAME_INWAITING)
        {
            sm0++;
        }
    }

    if (sm0 == 1)
    {
        if (fuelsensor.request.bf.pending == 1)//command waiting?
        {
            if (fuelsensor.request.cmd == FS_CMD_CODE_FULLRANGE_ADJ)
            {
                fuelsensor.request.response_ext = jt606_send_fullrangeAdj(JT606_CMD_SET);
            }
            else if (fuelsensor.request.cmd == FS_CMD_CODE_ZERO_ADJ)
            {
                fuelsensor.request.response_ext = jt606_send_zeroAdj(JT606_CMD_SET);
            }

            if (fuelsensor.request.response_ext > FS_COD_RET_FRAME_INWAITING)
            {
                //la aplicacion que demando el request limpiara el valor
                //fuelsensor.request.response_ext = FS_COD_RET_CLEAR;
                fuelsensor.request.bf.pending = 0;
                sm0 = 0x00;
            }
        }
        else
        {
            sm0 = 0x00;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////
void fuelsensor_job(void)
{
    float fuelsensor_value=0.0f;//format value

    float h;
    float Hx;

    if ( (!fuelsensor.fsEE.bf.full_calib) || (!fuelsensor.fsEE.bf.zero_calib) )//No calibrado?
    {
        fuelsensor.value = FS_VALUE_ERROR_NO_CALIBRATED;
    }
    else
    {
        //Can return:
        // #define FS_COD_RET_FRAME_INWAITING  0
        // #define FS_COD_RET_FRAME_CRC_BAD 3
        // #define FS_COD_RET_FRAME_TIMEOUT 4
        // #define FS_COD_RET_FRAME_ERROR 5
        //#define FS_COD_RET_FRAME_OFFTIME_OR_LOST 6
        //#define FS_COD_RET_FRAME_CYCLE_TXRX_OK 7
        if (cod_ret_readfl != FS_COD_RET_FRAME_INWAITING)
        {
            if (cod_ret_readfl == FS_COD_RET_FRAME_CYCLE_TXRX_OK)
            {
                //x defect. length of units = Centimeters
                h = fuelsensor.fsEE.calib.tank.zero2full * (jt606.internalSensor.percentage/100);
                Hx = fuelsensor.fsEE.calib.tank.innertank - ( (fuelsensor.fsEE.calib.tank.zero2full + fuelsensor.fsEE.calib.tank.full2upper) - h);
                //

                if (fuelsensor.fsEE.outputType.type == (FS_OUTPUTTYPE)LENGTH)
                {
                    if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_PERCENTAGE)
                    {
                        fuelsensor_value = jt606.internalSensor.percentage;
                    }
                    else
                    {
                        if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_CENTIMETERS)
                        {
                            fuelsensor_value = Hx;
                        }
                        else if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_METERS)
                        {
                            Hx /= 100.0;
                            fuelsensor_value = Hx;
                        }
                    }
                }
                else if (fuelsensor.fsEE.outputType.type == (FS_OUTPUTTYPE)VOLUME)
                {
                    if (fuelsensor.fsEE.tank.type == FS_TANK_TYPE_RECTANGULAR)
                    {
                        fuelsensor_value = Hx *fuelsensor.fsEE.tank.rectangular.area.length * fuelsensor.fsEE.tank.rectangular.area.width * 1E-6;//m3

                        if (fuelsensor.fsEE.outputType.volume.units == FS_UNITMEA_VOLUME_GALLONS)
                        {
                            fuelsensor_value *= 264.17;  //1 m3 = 264.17 gal
                        }
                        else //liters
                        {
                            fuelsensor_value *= 1000;   //1m3 = 1000l
                        }
                    }
                    else//IRREGULAR
                    {
                        //spline
                        fuelsensor_value = spline_poly_eval(Hx);//spline_poly_eval(38);

                        if (fuelsensor_value != FS_VALUE_ERROR_SPLINE_OUT_LIMITS) //>= 0)
                        {
                            if (fuelsensor.fsEE.outputType.volume.units == FS_UNITMEA_VOLUME_GALLONS)
                            {
                                if (fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units == FS_UNITMEA_VOLUME_LITERS)
                                {
                                    fuelsensor_value *= 0.26417;//liters to gallons
                                }
                            }
                            else //liters
                            {
                                if (fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units == FS_UNITMEA_VOLUME_GALLONS)
                                {
                                    fuelsensor_value *= 3.79;//gal to liters
                                }
                            }
                        }
                    }
                }

                //fuelsensor.value = fuelsensor_value;
            }
            fuelsensor.value = fuelsensor_value;//if not connected...then show 0
        }
    }
}


void fuelsensor_reading(char *str_dest)
{
    //-------
    char unit[LCDAN_STR_MAX_SIZE];
    if (fuelsensor.fsEE.outputType.type == (FS_OUTPUTTYPE)LENGTH)
    {
        if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_PERCENTAGE)
            strcpy_P(unit, PSTR(" %"));
        else if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_CENTIMETERS)
            strcpy_P(unit, PSTR(" cm"));
        else if (fuelsensor.fsEE.outputType.length.units == FS_UNITMEA_LENGTH_METERS)
            strcpy_P(unit, PSTR(" m"));
    }
    else if (fuelsensor.fsEE.outputType.type == (FS_OUTPUTTYPE)VOLUME)
    {
        if (fuelsensor.fsEE.outputType.volume.units == FS_UNITMEA_VOLUME_GALLONS)
            strcpy_P(unit, PSTR(" gal"));
        else //liters
            strcpy_P(unit, PSTR(" L"));
    }


    if (fuelsensor.value >= 0)
    {
        //str_dest = "Reading: "+ String(fuelsensor.value) + unit;
        strcpy_P(str_dest, PSTR("Reading: "));

        #define FS_READING_DTOSTRF_WIDTH (1+4+1+2) //SIGN+NUM INT+.+DECIMALS
        #define FS_READING_DTOSTRF_SIZE_BUF (DTOSTRF_WIDTH+1)               //DTOSTRF_WIDTH+ NULL_TERMINATOR
        char char_buff[SYSPRINT_STR_MAX_SIZE];
        strcat(str_dest, dtostrf(fuelsensor.value, FS_READING_DTOSTRF_WIDTH, 2, char_buff) );

        strcat(str_dest, unit);
    }
    else
    {
        if (fuelsensor.value == FS_VALUE_ERROR_SPLINE_OUT_LIMITS)
        {
            //str_dest = "Reading: "+ String("OUT LIMITS");//OUT OF LIMITS
            strcpy_P(str_dest, PSTR("Reading: OUT LIMITS"));//OUT OF LIMITS

        }
        else if (fuelsensor.value == FS_VALUE_ERROR_NO_CALIBRATED)
        {
            //str_dest = "Reading: "+ String("-NO CALIB-");
            strcpy_P(str_dest, PSTR("Reading: -NO CALIB-"));//OUT OF LIMITS
        }
        else //if (fuelsensor.value == FS_VALUE_ERROR_NEGATIVE)
        {
            //str_dest = "Reading: "+ String("-NEGATIVE-");
            strcpy_P(str_dest, PSTR("Reading: -NEGATIVE-"));//OUT OF LIMITS
        }
    }

}
