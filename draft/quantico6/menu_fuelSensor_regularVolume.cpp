/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.50
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
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"
#include "menu.h"
#include "menu_fuelSensor.h"
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"
#include "menu_mainMenu.h"
#include "menu_fuelSensor_regularVolume.h"
#include "setnum.h"
#include "menu_fuelSensor_fuelTank.h"

uint8_t mFuelSensor_rectangularVolume_setnum_number_printxy_getFil(uint8_t selected)
{
    uint8_t vfil[2]={LCD_ROW_1,LCD_ROW_2};
    return vfil[selected];
}
uint8_t mFuelSensor_rectangularVolume_setnum_number_printxy_getCol(uint8_t selected)
{
    uint8_t vcol[2]={13,13};
    return vcol[selected];
}

#define LENGTH_SETNUM_CHARNUMBER_SELECTED 0
#define WIDTH_SETNUM_CHARNUMBER_SELECTED 1

#define LENGTH_ASCHAR_NUMINTEGERS 3
#define LENGTH_ASCHAR_NUMDECIMALS 2

#define WIDTH_ASCHAR_NUMINTEGERS 3
#define WIDTH_ASCHAR_NUMDECIMALS 2

//automatic
#define LENGTH_ASCHAR_BUFFWITDH (LENGTH_ASCHAR_NUMINTEGERS+1+LENGTH_ASCHAR_NUMDECIMALS)
#define LENGTH_ASCHAR_DIGPOS_DP LENGTH_ASCHAR_NUMINTEGERS
#define LENGTH_ASCHAR_DIGPOS_ENDOFDIGIT (LENGTH_ASCHAR_NUMINTEGERS+1+LENGTH_ASCHAR_NUMDECIMALS)-1

#define WIDTH_ASCHAR_BUFFWITDH (WIDTH_ASCHAR_NUMINTEGERS+1+WIDTH_ASCHAR_NUMDECIMALS)
#define WIDTH_ASCHAR_DIGPOS_DP WIDTH_ASCHAR_NUMINTEGERS
#define WIDTH_ASCHAR_DIGPOS_ENDOFDIGIT (WIDTH_ASCHAR_NUMINTEGERS+1+WIDTH_ASCHAR_NUMDECIMALS)-1


static inline void mFuelSensor_rectangularVolume_fxKey0(void)
{
}
static inline void mFuelSensor_rectangularVolume_fxKey1(void)
{
    setnum_charnumber_thenumber_digitDec();
}

static inline void mFuelSensor_rectangularVolume_fxKey2(void)
{
    setnum_charnumber_thenumber_digitInc();
}

static inline void mFuelSensor_rectangularVolume_fxKey3(void)
{
    setnum_charnumber_selectDigit_selectNumber();
}

static inline void mFuelSensor_rectangularVolume_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_RECTANGULARVOLUMEN_SAVE_AND_EXIT_JOB);
}

PTRFX_retVOID const mFuelSensor_rectangularVolume_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_rectangularVolume_fxKey0, mFuelSensor_rectangularVolume_fxKey1, mFuelSensor_rectangularVolume_fxKey2,
    mFuelSensor_rectangularVolume_fxKey3, mFuelSensor_rectangularVolume_fxKey4
};

void mFuelSensor_rectangularVolume_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_rectangularVolume_fxKey);

        lcd.clear();
        sysPrint_lcdPrintPSTR(0, 0, PSTR("[Dimensions in cm]"));
        sysPrint_lcdPrintPSTR(1, 3, PSTR("Length ="));
        sysPrint_lcdPrintPSTR(2, 3, PSTR("Width  ="));


        //setnum initialization
        setnum_digit_reset_charnumber_reset();
        setnum_digit_blink_set_timeout(500);//ms
        setnum_charnumber_qty_of_charnumber(2);
        setnum_charnumber_thenumber_digitposition_init(LENGTH_SETNUM_CHARNUMBER_SELECTED, LENGTH_ASCHAR_DIGPOS_DP, LENGTH_ASCHAR_DIGPOS_ENDOFDIGIT);
        setnum_charnumber_thenumber_digitposition_init(WIDTH_SETNUM_CHARNUMBER_SELECTED, WIDTH_ASCHAR_DIGPOS_DP, WIDTH_ASCHAR_DIGPOS_ENDOFDIGIT);
        setnum_printxy_register_pFil(mFuelSensor_rectangularVolume_setnum_number_printxy_getFil);
        setnum_printxy_register_pCol(mFuelSensor_rectangularVolume_setnum_number_printxy_getCol);
        //
        setnum_charnumber_set_charvalue_asDou_aftFillZeros(LENGTH_SETNUM_CHARNUMBER_SELECTED, fuelsensor.fsEE.tank.rectangular.area.length, LENGTH_ASCHAR_BUFFWITDH, LENGTH_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(LENGTH_SETNUM_CHARNUMBER_SELECTED);

        setnum_charnumber_set_charvalue_asDou_aftFillZeros(WIDTH_SETNUM_CHARNUMBER_SELECTED, fuelsensor.fsEE.tank.rectangular.area.width, WIDTH_ASCHAR_BUFFWITDH, WIDTH_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(WIDTH_SETNUM_CHARNUMBER_SELECTED);

        //
        thismenu_sm0 = -1;
    }
    //-------- blinking ----------
    setnum_digit_blinking();
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey0(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
    }
    else if (thismenu_sm0 == 3)
    {
    }
}
static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey1(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
        vscroll.sign_up();
    }
    else if (thismenu_sm0 == 3)
    {
        vscroll.sign_up();
    }
}
static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey2(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
        vscroll.sign_down();
    }
    else if (thismenu_sm0 == 3)
    {
        vscroll.sign_down();
    }
}
static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey3(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
        //"[DATA INCONSISTENT]"));
        //"Save data anaway?"));
        // or
        //"[DATA IS CONSISTENT]"));
        //"Save data ?"));

        thismenu_sm1 = 0;

        if ( vscroll.get_markPosc() == 0)//Yes
        {
            fuelsensor.fsEE.tank.rectangular.area.length = setnum_charnumber_get_charvalue_asDou(LENGTH_SETNUM_CHARNUMBER_SELECTED);
            fuelsensor.fsEE.tank.rectangular.area.width = setnum_charnumber_get_charvalue_asDou(WIDTH_SETNUM_CHARNUMBER_SELECTED);

            if ( ( fuelsensor.fsEE.tank.rectangular.area.length == 0.0f) || ( fuelsensor.fsEE.tank.rectangular.area.width == 0.0f) )
            {
                fuelsensor.fsEE.bf.rectangular_area_consistent = 0;
            }
            else
            {
                fuelsensor.fsEE.bf.rectangular_area_consistent = 1;
            }
            fuelsensor_check_consistency();
            fuelsensor_savedata();

            //send changes to server
            eth_app_datasend_stackPush(idx_fuelsensor_tank_rectangular_area_length);
            eth_app_datasend_stackPush(idx_fuelsensor_tank_rectangular_area_width);
            eth_app.send.bf.request = 1;
        }
        else //No
        {

        }
        thismenu_sm1 = 0;
        //vscroll.base_offset_reset();//no hay necesidad porque las opciones que vienen son de pregunta y no guardan contexto
        thismenu_sm0++;//show next menu
    }
    else if (thismenu_sm0 == 3)
    {
        //"[MESSAGE]"));
        //"Exit ?"));//then EXIT
        thismenu_sm1 = 0;

        if ( vscroll.get_markPosc() == 0)//Yes
        {
            menu_setSubMenu(MGO_MFUELSENSOR_TANK_JOB);
            vscroll.stack_pop();
        }
        else //No
        {
            menu_setSubMenu(MGO_MFUELSENSOR_RECTANGULARVOLUME_JOB);
        }
    }
}

static void mFuelSensor_rectangularVolumen_save_and_exit_fxKey4(void)
{
    if (thismenu_sm0 == 1)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
    }
    else if (thismenu_sm0 == 3)
    {
    }
}

PTRFX_retVOID const mFuelSensor_rectangularVolumen_save_and_exit_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_rectangularVolumen_save_and_exit_fxKey0, mFuelSensor_rectangularVolumen_save_and_exit_fxKey1, mFuelSensor_rectangularVolumen_save_and_exit_fxKey2,
    mFuelSensor_rectangularVolumen_save_and_exit_fxKey3, mFuelSensor_rectangularVolumen_save_and_exit_fxKey4
};

void mFuelSensor_rectangularVolumen_save_and_exit_job(void)
{
    float length;
    float  width;

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_rectangularVolumen_save_and_exit_fxKey);

        length = setnum_charnumber_get_charvalue_asDou(LENGTH_SETNUM_CHARNUMBER_SELECTED);
        width = setnum_charnumber_get_charvalue_asDou(WIDTH_SETNUM_CHARNUMBER_SELECTED);

        if ( ( length == 0.0f) || ( width == 0.0f) )
            thismenu_sm0  = 1;
        else
            thismenu_sm0  = 2;

        thismenu_sm1 = 0;
    }

    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[WARNING]"));

            String msg;

            if ( length == 0.0f)
                msg+="[Length] ";

            if ( width == 0.0f)
                msg+="[Width]";

            sysPrint_lcdPrint(LCD_ROW_1, STR_CENTER, msg);
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("must be > 0"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("to get volume"));

            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 >= 2)
    {
        String db_vscroll[2] =
        {
            str_clearPrint("1] Yes", 7),
            str_clearPrint("2] No", 7),
        };

        if (thismenu_sm0 == 2)
        {
            if (thismenu_sm1 == 0)
            {
                //vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);//base offset from Tank job
                vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, 0x00, 0x00);//without context push/pop

                lcd.clear();

                length = setnum_charnumber_get_charvalue_asDou(LENGTH_SETNUM_CHARNUMBER_SELECTED);
                width = setnum_charnumber_get_charvalue_asDou(WIDTH_SETNUM_CHARNUMBER_SELECTED);


                if ( ( length == 0.0f) || ( width == 0.0f) )
                {
                    sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[DATA INCONSISTENT]"));
                    sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Save data anaway?"));
                }
                else
                {
                    sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[DATA IS CONSISTENT]"));
                    sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Save data ?"));
                }
                thismenu_sm1 = -1;
            }
        }

        if (thismenu_sm0 == 3)
        {
            if (thismenu_sm1 == 0)
            {
                //vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
                vscroll.init(db_vscroll, 2, dispShowBuff, 2, FEAT_MARK_ON, 0x00, 0x00);//without context push/pop

                lcd.clear();
                sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[MESSAGE]"));
                sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Exit ?"));//then EXIT

                thismenu_sm1 = -1;
            }
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        lcd_show_disp(LCD_ROW_2);
    }
}
