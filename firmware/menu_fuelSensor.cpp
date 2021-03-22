/****************************************************************************
* Project: Battery System
*
* 2016-05-11-19.38
* jcaf @ jcafOpenSuse
*
* Copyright 2016 Juan Carlos AgÃ¼ero Flores
*
* Licensed under the Apache License, Version 2.0 (the "License"));
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

#include "sysprint.h"

#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"

#include "menu.h"
#include "menu_fuelSensor.h"
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"
#include "menu_mainMenu.h"

#include "menu_fuelSensor_fuelTank.h"
#include "menu_fuelSensor_calibration.h"
#include "menu_fuelSensor_irrTank_menu.h"
#include "menu_fuelSensor_modelOfSensor.h"
#include "menu_fuelSensor_outputTypeUnits.h"
#include "menu_fuelSensor_regularVolume.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_warning_fxKey0(void)
{
}
static inline void mFuelSensor_warning_fxKey1(void)
{
    if (--thismenu_sm1 < 0)
        thismenu_sm1 = 0;
    else
        thismenu_sm0 = 1;
}
static inline void mFuelSensor_warning_fxKey2(void)
{
    if (++thismenu_sm1 > 1)
    {
        thismenu_sm1 = 0;

        menu_setSubMenu(MGO_MFUELSENSOR_MENU_JOB);
    }
    else
        thismenu_sm0 = 1;
}
static inline void mFuelSensor_warning_fxKey3(void)
{
    mFuelSensor_warning_fxKey2();//only forward
}
static inline void mFuelSensor_warning_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setMenu(MGO_MAIN_MENU);
}

PTRFX_retVOID const mFuelSensor_warning_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_warning_fxKey0, mFuelSensor_warning_fxKey1, mFuelSensor_warning_fxKey2,
    mFuelSensor_warning_fxKey3, mFuelSensor_warning_fxKey4
};

void mFuelSensor_warning_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_warning_fxKey);

        thismenu_sm1 = 0;
        thismenu_sm0++;
    }

    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[WARNING]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("This sensor use"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("diesel, biodisel,"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("kerosene, gasoline"));
        }

        else if (thismenu_sm1 == 1)
        {
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Do not measure"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("conductive material"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("like water, etc."));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("(Press any key...)"));
        }

        thismenu_sm0 = -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_menu_fxKey0(void)
{
}
static inline void mFuelSensor_menu_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_menu_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mFuelSensor_menu_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0:menu_setSubMenu(MGO_MFUELSENSOR_MODELOFSENSOR_JOB);
        break;

        case 1:menu_setSubMenu(MGO_MFUELSENSOR_CALIBRATION_JOB);
        break;

        case 2:menu_setSubMenu(MGO_MFUELSENSOR_TANK_JOB);
        break;

        case 3:menu_setSubMenu(MGO_MFUELSENSOR_OUTPUTTYPE_JOB);
        break;

    }
}
static inline void mFuelSensor_menu_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setMenu(MGO_MAIN_MENU);
}

PTRFX_retVOID const mFuelSensor_menu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_menu_fxKey0, mFuelSensor_menu_fxKey1, mFuelSensor_menu_fxKey2,
    mFuelSensor_menu_fxKey3, mFuelSensor_menu_fxKey4
};

void mFuelSensor_menu_job(void)
{
#define MENU_FUELSENSOR_OPTIONS 4

    char db_mFuelSensor_menu[MENU_FUELSENSOR_OPTIONS][LCDAN_STR_MAX_SIZE];

    lcdan_str_lineformat_align_P(&db_mFuelSensor_menu[0][0],  PSTR("1] MODEL OF SENSOR"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_menu[1][0],  PSTR("2] CALIBRATION"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_menu[2][0],  PSTR("3] FUEL TANK"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_menu[3][0],  PSTR("4] OUTPUT TYPE"), 0);

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_menu_fxKey);
        vscroll.init(db_mFuelSensor_menu, MENU_FUELSENSOR_OPTIONS, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[FUEL SENSOR]"));

        thismenu_sm0 =-1;
    }
    char str_dest[SYSPRINT_STR_MAX_SIZE];
    fuelsensor_reading(str_dest);
    sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_1, 0, str_dest);//comment by now

    vscroll.set_db(db_mFuelSensor_menu);//x local db
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}


