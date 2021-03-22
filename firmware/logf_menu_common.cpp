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
#include "ikey.h"
#include "kb.h"
#include "lcd_fxaux.h"
#include "vscroll.h"
#include "sysprint.h"
#include "logf_class.h"
#include "logf.h"
#include "menu.h"
#include "menu_logf_newFx.h"
#include "setnum.h"
#include "piz_class.h"
#include "piz.h"
#include <EEPROM.h>
#include "myEEPROM.h"


static struct _mLogf_common
{
    int8_t sm0;
    int8_t sm1;
    int8_t opt_sel;
}mLogf_common[2];

static inline void mLogf_common_digInput_fxKey0(void)
{
}
static inline void mLogf_common_digInput_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_digInput_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mLogf_common_digInput_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_digInput_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}

PTRFX_retVOID const mLogf_common_digInput_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_digInput_fxKey0, mLogf_common_digInput_fxKey1, mLogf_common_digInput_fxKey2,
    mLogf_common_digInput_fxKey3, mLogf_common_digInput_fxKey4
};

int8_t mLogf_common_digInput_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {
        char db_digInput[NUM_DIG_INPUT_MAX][LCDAN_STR_MAX_SIZE];
        //
        char str[LCDAN_STR_MAX_SIZE];
        char buff[LCDAN_STR_MAX_SIZE];
        for (uint8_t i=0; i<NUM_RELE_MAX; i++)
        {
            itoa(i+1, buff, 10);
            strcpy(str, buff);
            strcat(str,"] D");
            strcat(str,buff);
            lcdan_str_lineformat_align(&db_digInput[i][0],  str, LCDAN_STR_FORMAT_ALIGN_CENTER);
        }
        //
        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_digInput_fxKey);
            vscroll.init(db_digInput, NUM_DIG_INPUT_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT VARIABLE]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_digInput);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_anInput_fxKey0(void)
{
}
static inline void mLogf_common_anInput_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_anInput_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_common_anInput_fxKey3(void)
{
    mLogf_common[0].opt_sel  = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_anInput_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}
PTRFX_retVOID const mLogf_common_anInput_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_anInput_fxKey0, mLogf_common_anInput_fxKey1, mLogf_common_anInput_fxKey2,
    mLogf_common_anInput_fxKey3, mLogf_common_anInput_fxKey4
};

int8_t mLogf_common_anInput_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {

        char db_anInput[NUM_ANALOG_INPUT_MAX][LCDAN_STR_MAX_SIZE];
        //
        char str[LCDAN_STR_MAX_SIZE];
        char buff[LCDAN_STR_MAX_SIZE];
        for (uint8_t i=0; i<NUM_RELE_MAX; i++)
        {
            itoa(i+1, buff, 10);
            strcpy(str, buff);
            strcat(str,"] A");
            strcat(str,buff);
            lcdan_str_lineformat_align(&db_anInput[i][0],  str, LCDAN_STR_FORMAT_ALIGN_CENTER);
        }
        //
        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_anInput_fxKey);
            vscroll.init(db_anInput, NUM_ANALOG_INPUT_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT VARIABLE]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_anInput);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3); //fil 2 y fil3 are printable
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }

    return cod_ret;
}
///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_relay_fxKey0(void)
{
}
static inline void mLogf_common_relay_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_relay_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_common_relay_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_relay_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}
PTRFX_retVOID const mLogf_common_relay_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_relay_fxKey0, mLogf_common_relay_fxKey1, mLogf_common_relay_fxKey2,
    mLogf_common_relay_fxKey3, mLogf_common_relay_fxKey4
};

int8_t mLogf_common_relay_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {
        char db_relay[NUM_RELE_MAX][LCDAN_STR_MAX_SIZE];
        //
        char str[LCDAN_STR_MAX_SIZE];
        char buff[LCDAN_STR_MAX_SIZE];
        for (uint8_t i=0; i<NUM_RELE_MAX; i++)
        {
            itoa(i+1, buff, 10);
            strcpy(str, buff);
            strcat(str,"] R");
            strcat(str,buff);
            lcdan_str_lineformat_align(&db_relay[i][0],  str, LCDAN_STR_FORMAT_ALIGN_CENTER);
        }
        //
        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_relay_fxKey);
            vscroll.init(db_relay, NUM_RELE_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT VARIABLE]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_relay);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3); //fil 2 y fil3 are printable
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}
///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_logicalOP_fxKey0(void)
{
}
static inline void mLogf_common_logicalOP_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_logicalOP_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_common_logicalOP_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_logicalOP_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}
PTRFX_retVOID const mLogf_common_logicalOP_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_logicalOP_fxKey0, mLogf_common_logicalOP_fxKey1, mLogf_common_logicalOP_fxKey2,
    mLogf_common_logicalOP_fxKey3, mLogf_common_logicalOP_fxKey4
};

int8_t mLogf_common_logicalOP_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {

#define LOGF_SYMB_LOGICAL_OP_NUM_MAX 3
        char db_logical_OP[LOGF_SYMB_LOGICAL_OP_NUM_MAX][LCDAN_STR_MAX_SIZE];
        //
        lcdan_str_lineformat_align_P(&db_logical_OP[0][0], PSTR("1] AND"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_logical_OP[1][0], PSTR("2] OR "), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_logical_OP[2][0], PSTR("3] NOT"), LCDAN_STR_FORMAT_ALIGN_CENTER);

        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_logicalOP_fxKey);
            vscroll.init(db_logical_OP, LOGF_SYMB_LOGICAL_OP_NUM_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT OPERATOR]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_logical_OP);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }

    return cod_ret;
}
///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_relationalOP_fxKey0(void)
{
}
static inline void mLogf_common_relationalOP_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_relationalOP_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_common_relationalOP_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_relationalOP_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}
PTRFX_retVOID const mLogf_common_relationalOP_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_relationalOP_fxKey0, mLogf_common_relationalOP_fxKey1, mLogf_common_relationalOP_fxKey2,
    mLogf_common_relationalOP_fxKey3, mLogf_common_relationalOP_fxKey4
};

int8_t mLogf_common_relationalOP_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {

#define LOGF_SYMB_RELATIONAL_OP_NUM_MAX 6
        char db_relational_OP[LOGF_SYMB_RELATIONAL_OP_NUM_MAX][LCDAN_STR_MAX_SIZE];
        //
        lcdan_str_lineformat_align_P(&db_relational_OP[0][0], PSTR("1] =="), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_relational_OP[1][0], PSTR("2] !="), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_relational_OP[2][0], PSTR("3] < "), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_relational_OP[3][0], PSTR("4] > "), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_relational_OP[4][0], PSTR("5] <="),  LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_relational_OP[5][0], PSTR("6] >="), LCDAN_STR_FORMAT_ALIGN_CENTER);

        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_relationalOP_fxKey);
            vscroll.init(db_relational_OP, LOGF_SYMB_RELATIONAL_OP_NUM_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT OPERATOR]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_relational_OP);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}
///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_arithmeticOP_fxKey0(void)
{
}
static inline void mLogf_common_arithmeticOP_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_arithmeticOP_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_common_arithmeticOP_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_arithmeticOP_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}
PTRFX_retVOID const mLogf_common_arithmeticOP_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_arithmeticOP_fxKey0, mLogf_common_arithmeticOP_fxKey1, mLogf_common_arithmeticOP_fxKey2,
    mLogf_common_arithmeticOP_fxKey3, mLogf_common_arithmeticOP_fxKey4
};

int8_t mLogf_common_arithmeticOP_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {

#define LOGF_SYMB_ARITHMETIC_OP_NUM_MAX 4
        char db_arithmetic_OP[LOGF_SYMB_ARITHMETIC_OP_NUM_MAX][LCDAN_STR_MAX_SIZE];
        //
        lcdan_str_lineformat_align_P(&db_arithmetic_OP[0][0], PSTR("1] +"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_arithmetic_OP[1][0], PSTR("2] -"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_arithmetic_OP[2][0], PSTR("3] *"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_arithmetic_OP[3][0], PSTR("4] /"), LCDAN_STR_FORMAT_ALIGN_CENTER);


        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_arithmeticOP_fxKey);
            vscroll.init(db_arithmetic_OP, LOGF_SYMB_ARITHMETIC_OP_NUM_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT OPERATOR]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_arithmetic_OP);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_asociativityOP_fxKey0(void)
{
}
static inline void mLogf_common_asociativityOP_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_asociativityOP_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_common_asociativityOP_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_asociativityOP_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}
PTRFX_retVOID const mLogf_common_asociativityOP_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_asociativityOP_fxKey0, mLogf_common_asociativityOP_fxKey1, mLogf_common_asociativityOP_fxKey2,
    mLogf_common_asociativityOP_fxKey3, mLogf_common_asociativityOP_fxKey4
};

int8_t mLogf_common_asociativityOP_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {

#define LOGF_SYMB_ASSOCIATIVITY_OP_NUM_MAX 6
        char db_associativity_OP[LOGF_SYMB_ASSOCIATIVITY_OP_NUM_MAX][LCDAN_STR_MAX_SIZE];
        //
        lcdan_str_lineformat_align_P(&db_associativity_OP[0][0], PSTR("1] ("), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_associativity_OP[1][0], PSTR("2] )"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_associativity_OP[2][0], PSTR("3] ["), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_associativity_OP[3][0], PSTR("4] ]"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_associativity_OP[4][0], PSTR("5] {"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_associativity_OP[5][0], PSTR("6] }"), LCDAN_STR_FORMAT_ALIGN_CENTER);


        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_asociativityOP_fxKey);
            vscroll.init(db_associativity_OP, LOGF_SYMB_ASSOCIATIVITY_OP_NUM_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT OPERATOR]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_associativity_OP);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_punctuationmarkOP_fxKey0(void)
{
}
static inline void mLogf_common_punctuationmarkOP_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_punctuationmarkOP_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_common_punctuationmarkOP_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_punctuationmarkOP_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}
PTRFX_retVOID const mLogf_common_punctuationmarkOP_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_punctuationmarkOP_fxKey0, mLogf_common_punctuationmarkOP_fxKey1, mLogf_common_punctuationmarkOP_fxKey2,
    mLogf_common_punctuationmarkOP_fxKey3, mLogf_common_punctuationmarkOP_fxKey4
};

int8_t mLogf_common_punctuationmarkOP_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {

#define LOGF_SYMB_PUNCTUATIONMARK_OP_NUM_MAX 3
        char db_punctuationMark_OP[LOGF_SYMB_PUNCTUATIONMARK_OP_NUM_MAX][LCDAN_STR_MAX_SIZE];
        //
        lcdan_str_lineformat_align_P(&db_punctuationMark_OP[0][0], PSTR("1] ,"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_punctuationMark_OP[1][0], PSTR("2] ;"), LCDAN_STR_FORMAT_ALIGN_CENTER);
        lcdan_str_lineformat_align_P(&db_punctuationMark_OP[2][0], PSTR("3] :"), LCDAN_STR_FORMAT_ALIGN_CENTER);

        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_punctuationmarkOP_fxKey);
            vscroll.init(db_punctuationMark_OP, LOGF_SYMB_PUNCTUATIONMARK_OP_NUM_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT OPERATOR]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_punctuationMark_OP);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_assignmentOP_fxKey0(void)
{
}
static inline void mLogf_common_assignmentOP_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_assignmentOP_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_common_assignmentOP_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_assignmentOP_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}
PTRFX_retVOID const mLogf_common_assignmentOP_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_assignmentOP_fxKey0, mLogf_common_assignmentOP_fxKey1, mLogf_common_assignmentOP_fxKey2,
    mLogf_common_assignmentOP_fxKey3, mLogf_common_assignmentOP_fxKey4
};

int8_t mLogf_common_assignmentOP_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1 = 0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {
        #define LOGF_SYMB_ASSIGNMENT_OP_NUM_MAX 1
        char db_assignment_OP[LOGF_SYMB_ASSIGNMENT_OP_NUM_MAX][LCDAN_STR_MAX_SIZE];
        //
        lcdan_str_lineformat_align_P(&db_assignment_OP[0][0], PSTR("1] ="), LCDAN_STR_FORMAT_ALIGN_CENTER);

        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_assignmentOP_fxKey);
            vscroll.init(db_assignment_OP, LOGF_SYMB_ASSIGNMENT_OP_NUM_MAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[SELECT OPERATOR]"));
            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_assignment_OP);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_1);
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
//menu common
uint8_t mLogf_common_literal_setnum_number_printxy_getFil(uint8_t selected)
{
    uint8_t vfil[1] = {LCD_ROW_2};
    return vfil[selected];
}
uint8_t mLogf_common_literal_setnum_number_printxy_getCol(uint8_t selected)
{
    uint8_t vcol[1] = {5};
    return vcol[selected];
}
//->move to logf_class.h
//#define LITERAL_SETNUM_CHARNUMBER_SELECTED 0 // the name/orden of the number NAME_setnum
//
//#define LITERAL_ASCHAR_NUMINTEGERS 5
//#define LITERAL_ASCHAR_NUMDECIMALS 2
//
//// automatic
//#define LITERAL_ASCHAR_BUFFWITDH (LITERAL_ASCHAR_NUMINTEGERS + 1 + LITERAL_ASCHAR_NUMDECIMALS)
//#define LITERAL_ASCHAR_DIGPOS_DP LITERAL_ASCHAR_NUMINTEGERS
//#define LITERAL_ASCHAR_DIGPOS_ENDOFDIGIT (LITERAL_ASCHAR_NUMINTEGERS + 1 + LITERAL_ASCHAR_NUMDECIMALS) - 1
int8_t mLogf_literal_sm0;
int8_t mLogf_literal_sm1;
float mLogf_literal_valf;
static inline void mLogf_common_literal_fxKey0(void)
{
}
static inline void mLogf_common_literal_fxKey1(void)
{
    setnum_charnumber_thenumber_digitDec();
}

static inline void mLogf_common_literal_fxKey2(void)
{
    setnum_charnumber_thenumber_digitInc();
}

static inline void mLogf_common_literal_fxKey3(void)
{
    setnum_charnumber_selectDigit_selectNumber();
}

static inline void mLogf_common_literal_fxKey4(void)
{
    mLogf_literal_sm0 = 2;//exit
}

PTRFX_retVOID const mLogf_common_literal_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_literal_fxKey0, mLogf_common_literal_fxKey1,mLogf_common_literal_fxKey2, mLogf_common_literal_fxKey3,
    mLogf_common_literal_fxKey4
};

int8_t mLogf_common_literal_job(float *valf_dest)
{
    int8_t opt_dest;
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    int8_t menu_gui_event;

    if (mLogf_literal_sm0 == 0)
    {
        mLogf_literal_sm1 = 0;
        mLogf_literal_sm0++;

        // setnum initialization
        setnum_digit_reset_charnumber_reset();
        setnum_digit_blink_set_timeout(500); // ms
        setnum_charnumber_qty_of_charnumber(1);
        setnum_charnumber_thenumber_digitposition_init(LITERAL_SETNUM_CHARNUMBER_SELECTED, LITERAL_ASCHAR_DIGPOS_DP, LITERAL_ASCHAR_DIGPOS_ENDOFDIGIT);

        setnum_printxy_register_pFil(mLogf_common_literal_setnum_number_printxy_getFil);
        setnum_printxy_register_pCol(mLogf_common_literal_setnum_number_printxy_getCol);
        //
        float literal=00000.00f;
        setnum_charnumber_set_charvalue_asDou_aftFillZeros(LITERAL_SETNUM_CHARNUMBER_SELECTED,literal, LITERAL_ASCHAR_BUFFWITDH, LITERAL_ASCHAR_NUMDECIMALS);
    }
    if (mLogf_literal_sm0 == 1)
    {
        if (mLogf_literal_sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_literal_fxKey);
            lcd.clear();
            sysPrintLCDan_XY_P(LCD_ROW_0,0, PSTR("[ENTER LITERAL]"));

            setnum_charnumber_print_charvalue(LITERAL_SETNUM_CHARNUMBER_SELECTED);

            //
            mLogf_literal_sm1 = -1;
        }
        //-------- blinking ----------
        setnum_digit_blinking();
    }
    else if (mLogf_literal_sm0 == 2)
    {
        menu_gui_event = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Exit?"), &opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == GENERIC_YESNO_SELECT_YES)
                {
                    mLogf_literal_sm0 = 3;
                }
                else//GENERIC_YESNO_SELECT_NO
                {
                    mLogf_literal_sm0 = 1;
                    mLogf_literal_sm1 = 0;
                }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
               mLogf_literal_sm0 = 1;
               mLogf_literal_sm1 = 0;
            }
        }
    }
    //
    if (mLogf_literal_sm0 == 3)
    {
        //
        int8_t menu_gui_event = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Accept the number?"), &opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == GENERIC_YESNO_SELECT_YES)
                {
                    mLogf_literal_sm0 = 4;
                }
                else//GENERIC_YESNO_SELECT_NO
                {
                    cod_ret = MENU_GUI_EVENT_ESCAPE;
                    mLogf_literal_sm0 = 5;
                }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                mLogf_literal_sm0 = 2;
            }

        }
        //
    }
    if (mLogf_literal_sm0 == 4)
    {
        *valf_dest = setnum_charnumber_get_charvalue_asDou(LITERAL_SETNUM_CHARNUMBER_SELECTED);

        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;

        mLogf_literal_sm0 = 5;


    }
    if (mLogf_literal_sm0 == 5)
    {
        mLogf_literal_sm0 = 0x00;
    }

    return cod_ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_assign_relays_available_fxKey0(void)
{
}
static inline void mLogf_common_assign_relays_available_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_assign_relays_available_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mLogf_common_assign_relays_available_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_assign_relays_available_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}

PTRFX_retVOID const mLogf_common_assign_relays_available_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_assign_relays_available_fxKey0, mLogf_common_assign_relays_available_fxKey1,
    mLogf_common_assign_relays_available_fxKey2,    mLogf_common_assign_relays_available_fxKey3, mLogf_common_assign_relays_available_fxKey4
};

int8_t mLogf_common_assign_relays_available_job(int8_t *opt_dest, int8_t piz_action_subtype)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;

    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1=0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {
        //
        char db_logf_assignFx[NUM_RELE_MAX*2][LCDAN_STR_MAX_SIZE];
        char str[LCDAN_STR_MAX_SIZE];
        char buff[LCDAN_STR_MAX_SIZE];
        char relayn[LCDAN_STR_MAX_SIZE];
        uint8_t num_relay_not_busy=0;
        uint8_t vscroll_dispShowBuff_numline=2;

        uint8_t db_idx=0;

        //added
        if ( (piz_action_subtype == PIZ_SYMBOL_ASSIGN_ACTION_REPLACE)  || (piz_action_subtype == PIZ_SYMBOL_ASSIGN_ACTION_INSERT) )
        {
            for (uint8_t i=0; i<NUM_RELE_MAX; i++)
            {
                EEPROM.get(EEPROM_ADDR_RELE_BUSY + ( sizeof(struct _logf_rele_bf) * i ) , logf_rele[i].bf );
            }
        }
        //
        for (uint8_t i=0; i<NUM_RELE_MAX; i++)
        {
            if (!logf_rele[i].bf.busy)
            {
                itoa(i+1, buff, 10);
                strcpy(relayn,buff);
                //
                itoa(db_idx+1, buff, 10);
                strcpy(str, buff);
                strcat_P(str, PSTR("]R"));
                strcat(str, relayn);
                strcat_P(str, PSTR(" (Act.if true)"));
                lcdan_str_lineformat_align(&db_logf_assignFx[db_idx++][0], str, 0);
                //
                itoa(db_idx+1, buff, 10);
                strcpy(str, buff);
                strcat_P(str, PSTR("]R"));
                strcat(str, relayn);
                strcat_P(str, PSTR(" (Act.if false)"));
                lcdan_str_lineformat_align(&db_logf_assignFx[db_idx++][0], str, 0);

                num_relay_not_busy +=2;
            }
        }
        if (num_relay_not_busy > 0)
        {
            if (num_relay_not_busy>2)
            {
                vscroll_dispShowBuff_numline = 3;
            }
            if (mLogf_common[0].sm1 == 0)
            {
                kb_change_keyDo_pgm(mLogf_common_assign_relays_available_fxKey);
                vscroll.init(db_logf_assignFx, num_relay_not_busy, vscroll_dispShowBuff, vscroll_dispShowBuff_numline, FEAT_MARK_ON, 0, 0);
                lcd.clear();
                sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[ASSIGNMENT RELAYS]"));
                mLogf_common[0].sm1 = -1;
            }
            vscroll.set_db(db_logf_assignFx);
            vscroll.job();
            vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_1+vscroll_dispShowBuff_numline-1 );//
        }
        else
        {
            mLogf_common[0].sm0 = 3;//salimos directamente
        }
    }
    //else
    if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    //else
    if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}

//////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_assign_fx_timer_fxKey0(void)
{
}
static inline void mLogf_common_assign_fx_timer_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_assign_fx_timer_fxKey2(void)
{
    vscroll.sign_down();
}
static int8_t mLogf_common_assign_fx_timer_select=0;

static inline void mLogf_common_assign_fx_timer_fxKey3(void)
{

    if (mLogf_common_assign_fx_timer_select == 0)
    {
        mLogf_common[1].sm0 = 1;    //1)mostrar menu de Reles disponibles
        mLogf_common_assign_fx_timer_select++;
    }
    else if (mLogf_common_assign_fx_timer_select == 1)
    {
        mLogf_common[1].sm0 = 2;    //2)ingresar literal
        mLogf_common_assign_fx_timer_select++;
    }
    else if (mLogf_common_assign_fx_timer_select == 2)
    {
        mLogf_common[1].sm0 = 3;    //acepta el ingreso "<"
        mLogf_common_assign_fx_timer_select= 0;
    }
}
static inline void mLogf_common_assign_fx_timer_fxKey4(void)
{
    mLogf_common[1].sm0 = 4;
}

PTRFX_retVOID const mLogf_common_assign_fx_timer_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_assign_fx_timer_fxKey0, mLogf_common_assign_fx_timer_fxKey1, mLogf_common_assign_fx_timer_fxKey2,
    mLogf_common_assign_fx_timer_fxKey3, mLogf_common_assign_fx_timer_fxKey4
};

int8_t mLogf_common_assign_fx_timer_job(uint8_t *rele_dest, float *valf_dest, int8_t piz_action_subtype)
{
    int8_t menu_gui_event;
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;

    static int8_t opt_dest;
    static float valf;

    if (mLogf_common[1].sm0 == 0)
    {
        kb_change_keyDo_pgm(mLogf_common_assign_fx_timer_fxKey);

        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[TIMER]"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Timer(Rx, sec)"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Timer(Rx, 00000.00)"));

        mLogf_common[1].sm0 = -1;
    }
    else if (mLogf_common[1].sm0 ==1)//ingreso de Rele
    {
        menu_gui_event = mLogf_common_assign_relays_available_job(&opt_dest, piz_action_subtype);
        if ( menu_gui_event  != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                //show el Rele ingresado
            }
            else  if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                mLogf_common_assign_fx_timer_select = 0x0;//stay in the same place
            }
            mLogf_common[1].sm0 = 0;
        }
    }
    else if (mLogf_common[1].sm0 == 2)
    {
        menu_gui_event = mLogf_common_literal_job(&valf);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                //show el numero ingresado
            }
            else  if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                //show 0000.00
                //mostrar <, ese momento es la oportunidad para ingresar la funcion
            }
            mLogf_common[1].sm0 = 0;
        }
    }
    else if (mLogf_common[1].sm0 == 3)
    {
        *rele_dest = opt_dest;
        *valf_dest = valf;

        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[1].sm0 = 0x00;
    }
    else if (mLogf_common[1].sm0 == 4)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[1].sm0 = 0x00;
    }

    return cod_ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_select_logexp_fxKey0(void)
{
}
static inline void mLogf_common_select_logexp_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_common_select_logexp_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mLogf_common_select_logexp_fxKey3(void)
{
    mLogf_common[0].opt_sel = vscroll.get_markPosc();
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_select_logexp_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}

PTRFX_retVOID const mLogf_common_select_logexp_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_select_logexp_fxKey0, mLogf_common_select_logexp_fxKey1, mLogf_common_select_logexp_fxKey2, mLogf_common_select_logexp_fxKey3, mLogf_common_select_logexp_fxKey4
};

int8_t mLogf_common_select_logexp_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    char str[LCDAN_STR_MAX_SIZE];
    char buff[4];
    int8_t db_select_logexp_size = num_exp_saved;

    char db_select_logexp[db_select_logexp_size][LCDAN_STR_MAX_SIZE];
    int8_t vscroll_dispShowBuff_numline;
    //
    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm1=0;
        mLogf_common[0].sm0++;
    }
    if (mLogf_common[0].sm0 == 1)
    {
        for (uint8_t i=0; i<db_select_logexp_size; i++)
        {
            itoa(i+1, buff, 10);
            strcpy(str,buff);
            strcat_P(str, PSTR("] Logical Exp."));
            strcat(str, buff);
            lcdan_str_lineformat_align(&db_select_logexp[i][0], str, 0);
        }
        if (db_select_logexp_size > 3)
            vscroll_dispShowBuff_numline = 3;
        else
            vscroll_dispShowBuff_numline = db_select_logexp_size;
        //
        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_select_logexp_fxKey);
            vscroll.init(db_select_logexp, db_select_logexp_size, vscroll_dispShowBuff, vscroll_dispShowBuff_numline, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[SELECT LOG.EXP]"));

            mLogf_common[0].sm1 = -1;
        }
        vscroll.set_db(db_select_logexp);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_1+vscroll_dispShowBuff_numline-1 );//
    }
    else if (mLogf_common[0].sm0 == 2)
    {
        *opt_dest = mLogf_common[0].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_common_view_logExp_fxKey0(void)
{
}
static inline void mLogf_common_view_logExp_fxKey1(void)
{
    piz.vp.pvscroll->sign_up();
}
static inline void mLogf_common_view_logExp_fxKey2(void)
{
    piz.vp.pvscroll->sign_down();
}
static inline void mLogf_common_view_logExp_fxKey3(void)
{
    mLogf_common[0].sm0 = 2;
}
static inline void mLogf_common_view_logExp_fxKey4(void)
{
    mLogf_common[0].sm0 = 3;
}

PTRFX_retVOID const mLogf_common_view_logExp_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_common_view_logExp_fxKey0, mLogf_common_view_logExp_fxKey1, mLogf_common_view_logExp_fxKey2, mLogf_common_view_logExp_fxKey3, mLogf_common_view_logExp_fxKey4
};

int8_t mLogf_common_view_logExp_job(uint8_t logexp)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    char str[LCDAN_STR_MAX_SIZE];
    char buff[4];

    if (mLogf_common[0].sm0 == 0)
    {
        mLogf_common[0].sm0++;
        mLogf_common[0].sm1 = 0;
        //
        EEPROM.get(EEPROM_ADDR_LOGF_LOGEXP_ASSIGOUT_DATA + (sizeof(struct _logf_logExp_assigOut_data) )*(logexp),
                logf_logExp_assigOut_data);//ya esta cargado toda la data para ser manipulada
        //
    }
    if (mLogf_common[0].sm0 == 1)
    {
        if (mLogf_common[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_common_view_logExp_fxKey);
            //
            piz.builder_content();

            if (piz.prop.numlines > 0)
            {
               if (piz.prop.numlines > 3)
                {
                    piz.vp.numline = 3;
                }
                else
                {
                    piz.vp.numline = piz.prop.numlines;
                }
                piz.vp.pvscroll->set_dispShowBuff_numline(piz.vp.numline);
                piz.vp.pvscroll->set_db_dbsize(piz.prop.numlines);
                piz.vp.pvscroll->set_db_base(0x00);
                piz.vp.pvscroll->set_db_offset(0x00);
            }
            //
            itoa(logexp+1, buff, 10);
            strcpy_P(str, PSTR("[VIEWER][LOG.EXP "));
            strcat(str, buff);
            strcat(str, "]");
            //
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_0, 0, str);

            mLogf_common[0].sm1 = -1;
        }
        piz.vp.pvscroll->job();
        vscroll_lcd_show_disp(LCD_ROW_1, (LCD_ROW_1+ piz.vp.numline-1) );

    }
    else if (mLogf_common[0].sm0 == 2)
    {
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        mLogf_common[0].sm0 = 0x00;
    }
    else if (mLogf_common[0].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        mLogf_common[0].sm0 = 0x00;
    }
    return cod_ret;
}
