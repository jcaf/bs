/****************************************************************************
* Project: Battery System
*
* 2016-10-04-15.25
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
#include "ikey.h"
#include "kb.h"
#include "lcd_bklight.h"
#include "lcd_fxaux.h"
#include "vscroll.h"
#include "sysprint.h"
#include "logf_class.h"
#include "logf.h"
#include "menu.h"
#include "logf_menu_common.h"
#include "menu_logf_config.h"
#include "myEEPROM.h"
#include "setnum.h"


#include "menu_logf_newFx.h"//comunes

int8_t mLogF_config_anInput_job(uint8_t an, float *scale_factor, float *offset);

static inline void mLogf_config_fxKey0(void)
{
}
static inline void mLogf_config_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_config_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_config_fxKey3(void)
{
    int8_t submen = vscroll.get_markPosc();
    switch (submen)
    {
        case 0:
              //  thismenu_sm0 = 2;
            break;
        case 1:
                thismenu_sm0 = 3;
vscroll.stack_push();
            break;
        case 2:
               // thismenu_sm0 = 4;
            break;
        default:
            break;
    }

    thismenu_sm1 = 0x00;
    //vscroll.stack_push();
}
static inline void mLogf_config_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MLOGF_MAIN_JOB);
}

PTRFX_retVOID const mLogf_config_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_config_fxKey0, mLogf_config_fxKey1, mLogf_config_fxKey2,
    mLogf_config_fxKey3, mLogf_config_fxKey4
};

void mLogf_config_job(void)
{
    static int8_t opt_dest;
    int8_t menu_gui_event;
    //int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;

    if (thismenu_sm0 == 0)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    if (thismenu_sm0 == 1)
    {
        char db_logf_config[3][LCDAN_STR_MAX_SIZE];

        lcdan_str_lineformat_align_P(&db_logf_config[0][0],  PSTR("1] Digital inputs"), 0);//opt_destionals
        lcdan_str_lineformat_align_P(&db_logf_config[1][0],  PSTR("2] Analog inputs"), 0);
        lcdan_str_lineformat_align_P(&db_logf_config[2][0],  PSTR("3] Output relays"), 0);//opt_destionals

        if (thismenu_sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_config_fxKey);

            vscroll.init(db_logf_config, 3, vscroll_dispShowBuff, 3, FEAT_MARK_ON, vscroll.base, vscroll.offset);

            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[CONFIG]"));

            thismenu_sm1 = -1;
        }
        vscroll.set_db(db_logf_config);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (thismenu_sm0 == 2)//digital inputs config
    {
        if (thismenu_sm1 == 0)
        {
            if (mLogf_common_digInput_job(&opt_dest) != MENU_GUI_EVENT_WAITING_ACTION)
            {
                thismenu_sm1++;
            }
        }
        if (thismenu_sm1 == 1)
        {
            thismenu_sm0 = 5;//return to menu
        }
    }
    else if (thismenu_sm0 == 3)//analog inputs config
    {
        if (thismenu_sm1 == 0)
        {
            menu_gui_event = mLogf_common_anInput_job(&opt_dest);
            if ( menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
            {
                if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
                {
                    thismenu_sm1++;
                }
                else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
                {
                    thismenu_sm0 = 5;
                }
            }
        }
        if (thismenu_sm1 == 1)
        {
            menu_gui_event = mLogF_config_anInput_job(opt_dest, &logf_analog_input[opt_dest].an_input_factors.scale, &logf_analog_input[opt_dest].an_input_factors.offset);
            if ( menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
            {
                if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
                {
                    //save in EEPROM
                    EEPROM.put(EEPROM_ADDR_ANINPUT_FACTORS_DATA + ( sizeof(struct _an_input_factors) * opt_dest ),
                               logf_analog_input[opt_dest].an_input_factors );

                    thismenu_sm0 = 3;
                    thismenu_sm1 = 0;
                }
                else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
                {
                    EEPROM.get(EEPROM_ADDR_ANINPUT_FACTORS_DATA + ( sizeof(struct _an_input_factors) * opt_dest ),
                               logf_analog_input[opt_dest].an_input_factors );
                    //
                    thismenu_sm0 = 3;
                    thismenu_sm1 = 0;
                }
            }
        }
    }
    else if (thismenu_sm0 == 4)
    {
        if (thismenu_sm1 == 0)
        {
            //if (mSelect_digInput_job(&opt_dest) != MENU_GUI_EVENT_WAITING_ACTION)
            {
                thismenu_sm1++;
            }
        }
        if (thismenu_sm1 == 1)
        {
            thismenu_sm0 = 5;//return to menu
        }
    }
    //
    if (thismenu_sm0 == 5)
    {
        thismenu_sm0 = 0;
        vscroll.stack_pop();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t mLogF_config_anInput_setnum_number_printxy_getFil(uint8_t selected)
{
    uint8_t vfil[2]= {LCD_ROW_2,LCD_ROW_3};
    return vfil[selected];
}
uint8_t mLogF_config_anInput_setnum_number_printxy_getCol(uint8_t selected)
{
    uint8_t vcol[2]= {6,7};
    return vcol[selected];
}
//
//#define SCALEFACTOR_SETNUM_CHARNUMBER_SELECTED 0
//#define OFFSET_SETNUM_CHARNUMBER_SELECTED 1
//
//#define SCALEFACTOR_ASCHAR_NUMINTEGERS 5
//#define SCALEFACTOR_ASCHAR_NUMDECIMALS 6
//
//#define OFFSET_ASCHAR_NUMINTEGERS 5
//#define OFFSET_ASCHAR_NUMDECIMALS 5
//
////automatic
//#define SCALEFACTOR_ASCHAR_BUFFWITDH (SCALEFACTOR_ASCHAR_NUMINTEGERS+1+SCALEFACTOR_ASCHAR_NUMDECIMALS)
//#define SCALEFACTOR_ASCHAR_DIGPOS_DP SCALEFACTOR_ASCHAR_NUMINTEGERS
//#define SCALEFACTOR_ASCHAR_DIGPOS_ENDOFDIGIT (SCALEFACTOR_ASCHAR_NUMINTEGERS+1+SCALEFACTOR_ASCHAR_NUMDECIMALS)-1
//
//#define OFFSET_ASCHAR_BUFFWITDH (OFFSET_ASCHAR_NUMINTEGERS+1+OFFSET_ASCHAR_NUMDECIMALS)
//#define OFFSET_ASCHAR_DIGPOS_DP OFFSET_ASCHAR_NUMINTEGERS
//#define OFFSET_ASCHAR_DIGPOS_ENDOFDIGIT (OFFSET_ASCHAR_NUMINTEGERS+1+OFFSET_ASCHAR_NUMDECIMALS)-1

static inline void mLogF_config_anInput_fxKey0(void)
{
}
static inline void mLogF_config_anInput_fxKey1(void)
{
    setnum_charnumber_thenumber_digitDec();
}

static inline void mLogF_config_anInput_fxKey2(void)
{
    setnum_charnumber_thenumber_digitInc();
}

static inline void mLogF_config_anInput_fxKey3(void)
{
    setnum_charnumber_selectDigit_selectNumber();
}

static inline void mLogF_config_anInput_fxKey4(void)
{
    gpmenu[0].sm0 = 2;
}

PTRFX_retVOID const mLogF_config_anInput_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogF_config_anInput_fxKey0, mLogF_config_anInput_fxKey1, mLogF_config_anInput_fxKey2, mLogF_config_anInput_fxKey3, mLogF_config_anInput_fxKey4
};

int8_t mLogF_config_anInput_job(uint8_t an, float *scale_factor, float *offset)
{
    int8_t opt_dest_dest;
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    int8_t menu_gui_event;
    char str[LCDAN_STR_MAX_SIZE];
    char buff[4];

    if (gpmenu[0].sm0 == 0)
    {
        gpmenu[0].sm1 = 0;
        gpmenu[0].sm0++;
           //step 1:setnum initialization
        setnum_digit_reset_charnumber_reset();
        setnum_digit_blink_set_timeout(500);//ms
        setnum_charnumber_qty_of_charnumber(2);
        //step 2 :init
        setnum_charnumber_thenumber_digitposition_init(SCALEFACTOR_SETNUM_CHARNUMBER_SELECTED, SCALEFACTOR_ASCHAR_DIGPOS_DP, SCALEFACTOR_ASCHAR_DIGPOS_ENDOFDIGIT);
        setnum_charnumber_thenumber_digitposition_init(OFFSET_SETNUM_CHARNUMBER_SELECTED, OFFSET_ASCHAR_DIGPOS_DP, OFFSET_ASCHAR_DIGPOS_ENDOFDIGIT);
        //step 3 :register xy
        setnum_printxy_register_pFil(mLogF_config_anInput_setnum_number_printxy_getFil);
        setnum_printxy_register_pCol(mLogF_config_anInput_setnum_number_printxy_getCol);
        //step 4:   setnum_charnumber_set_charvalue_asDou_aftFillZeros(uint8_t selected, double dnumber, signed char __witdh, unsigned char __prec)
        //          dtostrf_fill_zero(setnum.charnumber.thenumber[selected].charvalue);
        setnum_charnumber_set_charvalue_asDou_aftFillZeros(SCALEFACTOR_SETNUM_CHARNUMBER_SELECTED, *scale_factor , SCALEFACTOR_ASCHAR_BUFFWITDH, SCALEFACTOR_ASCHAR_NUMDECIMALS);
        setnum_charnumber_set_charvalue_asDou_aftFillZeros(OFFSET_SETNUM_CHARNUMBER_SELECTED, *offset, OFFSET_ASCHAR_BUFFWITDH, OFFSET_ASCHAR_NUMDECIMALS);
        //
    }
    if (gpmenu[0].sm0 == 1)
    {
        if (gpmenu[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogF_config_anInput_fxKey);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[ENTER FACTORS]"));
            //
            itoa(an+1, buff, 10);
            strcpy_P(str, PSTR("[AN"));
            strcat(str, buff);
            strcat_P(str, PSTR("*Scale+Offset]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, str);
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, 0, PSTR("Scale="));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, 0, PSTR("Offset="));
            //
            setnum_charnumber_print_charvalue(SCALEFACTOR_SETNUM_CHARNUMBER_SELECTED);
            setnum_charnumber_print_charvalue(OFFSET_SETNUM_CHARNUMBER_SELECTED);

            gpmenu[0].sm1 = -1;
        }
        //-------- blinking ----------
        setnum_digit_blinking();
    }
    else if (gpmenu[0].sm0 == 2)
    {
        menu_gui_event = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Exit?"), &opt_dest_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest_dest == GENERIC_YESNO_SELECT_YES)
                {
                    gpmenu[0].sm0 = 3;
                }
                else//GENERIC_YESNO_SELECT_NO
                {
                    gpmenu[0].sm0 = 1;
                    gpmenu[0].sm1 = 0;
                }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
               gpmenu[0].sm0 = 1;
               gpmenu[0].sm1 = 0;
            }
        }
    }
    if (gpmenu[0].sm0 == 3)
    {
        int8_t menu_gui_event = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Save changes?"), &opt_dest_dest);

        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest_dest == GENERIC_YESNO_SELECT_YES)
                {
                    gpmenu[0].sm0 = 4;
                }
                else//GENERIC_YESNO_SELECT_NO
                {
                    gpmenu[0].sm0 = 5;
                }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = 2;
            }
        }
    }
    if (gpmenu[0].sm0 == 4)
    {
        *scale_factor = setnum_charnumber_get_charvalue_asDou(SCALEFACTOR_SETNUM_CHARNUMBER_SELECTED);
        *offset = setnum_charnumber_get_charvalue_asDou(OFFSET_SETNUM_CHARNUMBER_SELECTED);
        //
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        gpmenu[0].sm0 = 0x00;
    }
    if (gpmenu[0].sm0 == 5)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        gpmenu[0].sm0 = 0x00;
    }

    return cod_ret;
}
