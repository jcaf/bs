/****************************************************************************
* Project: Battery System
*
* 2016-05-24-16.02
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

#include "ethernet.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_tank_fxKey0(void)
{
}
static inline void mFuelSensor_tank_fxKey1(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.sign_up();
    }
}
static inline void mFuelSensor_tank_fxKey2(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.sign_down();
    }
}
static inline void mFuelSensor_tank_fxKey3(void)
{
    if (1)//((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.stack_push();
        vscroll.base_offset_reset();

        switch (vscroll.get_markPosc())
        {
            case 0: menu_setSubMenu(MGO_MFUELSENSOR_TYPEOFTANK_JOB);break;
            case 1:
                    if (fuelsensor.fsEE.tank.type == 0)    //RECTANGULAR
                        menu_setSubMenu(MGO_MFUELSENSOR_RECTANGULARVOLUME_JOB);//taskman_fuelSensor.sm2 = 2;//Rectangular Volume Calc
                    else
                        menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_MENU_JOB);//taskman_fuelSensor.sm2 = 3;//Irregular Volume Calc - SPLINE
                break;
        }
    }
}
static inline void mFuelSensor_tank_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MFUELSENSOR_MENU_JOB);
}

PTRFX_retVOID const mFuelSensor_tank_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_tank_fxKey0, mFuelSensor_tank_fxKey1, mFuelSensor_tank_fxKey2,
    mFuelSensor_tank_fxKey3, mFuelSensor_tank_fxKey4
};
void mFuelSensor_tank_job(void)
{
    #define MENU_FUELSENSOR_TANK 2
    char db_mFuelSensor_tank_local[MENU_FUELSENSOR_TANK][LCDAN_STR_MAX_SIZE];

    lcdan_str_lineformat_align_P(&db_mFuelSensor_tank_local[0][0],  PSTR("[1] TYPE OF TANK"), 1);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_tank_local[1][0],  PSTR("[2] VOLUME CALC."), 1);

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_tank_fxKey);
        vscroll.init(db_mFuelSensor_tank_local, MENU_FUELSENSOR_TANK, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[FUEL TANK]"));

        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_tank_local);//x la naturaleza "local"
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_TYPEOFTANK 2
//__________________________________________________
const struct _fuelsensor_db_typeOfTank
{
    char name[15];
}
fuelsensor_db_typeOfTank[MENU_FUELSENSOR_TYPEOFTANK] PROGMEM=
{
    "RECTANGULAR",
    "IRREGULAR  ",
};
inline static void fuelsensor_get_typeOfTank_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_typeOfTank[type].name );
}
//__________________________________________________
static void mFuelSensor_typeOfTank_printCurrent(void)
{
    char buf[15];
    fuelsensor_get_typeOfTank_name(fuelsensor.fsEE.tank.type, buf);
    lcd.printAtPosc(1, 9, buf);
}

static inline void mFuelSensor_typeOfTank_fxKey0(void)
{
}
static inline void mFuelSensor_typeOfTank_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_typeOfTank_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mFuelSensor_typeOfTank_fxKey3(void)
{
    fuelsensor.fsEE.tank.type = vscroll.get_markPosc();//0=rectangular 1=irregular
    fuelsensor_check_consistency();
    fuelsensor_savedata();

    mFuelSensor_typeOfTank_printCurrent();
    //
    eth_app_datasend_stackPush(idx_fuelsensor_tank_type);
    eth_app.send.bf.request = 1;
}
static inline void mFuelSensor_typeOfTank_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_TANK_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_typeOfTank_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_typeOfTank_fxKey0, mFuelSensor_typeOfTank_fxKey1, mFuelSensor_typeOfTank_fxKey2,
    mFuelSensor_typeOfTank_fxKey3, mFuelSensor_typeOfTank_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
void mFuelSensor_typeOfTank_job(void)
{
    char buf[15];
    char db_mFuelSensor_typeOfTank_local[MENU_FUELSENSOR_TYPEOFTANK][LCDAN_STR_MAX_SIZE];

    char str[LCDAN_STR_MAX_SIZE];
    char str_itoa[LCDAN_STR_MAX_SIZE];

    for (uint8_t i=0; i<MENU_FUELSENSOR_TYPEOFTANK; i++)
    {
        fuelsensor_get_typeOfTank_name(i, buf);
        //db_mFuelSensor_typeOfTank_local[i] = str_clearPrint( String(i+1)+ "] "+ String(buf), 3);

        itoa(i+1, str_itoa, 10);
        strcpy(str, str_itoa );
        strcat_P(str, PSTR("] "));
        strcat(str, buf);

        lcdan_str_lineformat_align(&db_mFuelSensor_typeOfTank_local[i][0],  str, 3);
    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_typeOfTank_fxKey);
        vscroll.init(db_mFuelSensor_typeOfTank_local, MENU_FUELSENSOR_TYPEOFTANK, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[TYPE OF TANK]"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, 0, PSTR("Current: "));

        mFuelSensor_typeOfTank_printCurrent();

        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_typeOfTank_local);//x la naturaleza "local"
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}


