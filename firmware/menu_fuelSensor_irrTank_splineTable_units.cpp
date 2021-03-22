/****************************************************************************
* Project: Battery System
*
* 2016-07-11-19.57
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

#include "setnum.h"
#include "menu_fuelSensor_fuelTank.h"

#include "menu_fuelSensor.h"//x load EEPROM

#include "menu_fuelSensor_irrTank_common.h"
#include "menu_fuelSensor_irrTank_splineTable_modifNode.h"

#include "sysprint.h"
#include "ethernet.h"

#define MENU_FUELSENSOR_OUTPUTTYPE 2
const struct _fuelsensor_db_irregular_unitsOfTable
{
    char name[10];
}
fuelsensor_db_irregular_unitsOfTable[MENU_FUELSENSOR_OUTPUTTYPE] PROGMEM=
{
    "LENGTH",
    "VOLUME",
};

inline static void fuelsensor_get_irregular_unitsOfTable_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_irregular_unitsOfTable[type].name );
}


static inline void mFuelSensor_irrTank_spTable_units_fxKey0(void)
{
}
static inline void mFuelSensor_irrTank_spTable_units_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_irrTank_spTable_units_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mFuelSensor_irrTank_spTable_units_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0:
            menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_UNITS_LENGTHUNITS_JOB);
            break;

        case 1:
            menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_UNITS_VOLUMENUNITS_JOB);
            break;
    }
}
static inline void mFuelSensor_irrTank_spTable_units_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_MENU_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_irrTank_spTable_units_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_units_fxKey0, mFuelSensor_irrTank_spTable_units_fxKey1, mFuelSensor_irrTank_spTable_units_fxKey2,
    mFuelSensor_irrTank_spTable_units_fxKey3, mFuelSensor_irrTank_spTable_units_fxKey4
};

void mFuelSensor_irrTank_spTable_units_job(void)
{
    char buf[15];
    char db_mFuelSensor_irrTank_spTable_units[2][LCDAN_STR_MAX_SIZE];

    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];

    for (uint8_t i=0; i<2; i++)
    {

        fuelsensor_get_irregular_unitsOfTable_name(i, buf);
        //db_mFuelSensor_irrTank_spTable_units[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 5);
        //
        itoa(i+1, buff, 10);
        strcpy(str, buff);
        strcat_P(str, PSTR("] "));
        strcat(str, buf);
        //
        lcdan_str_lineformat_align(&db_mFuelSensor_irrTank_spTable_units[i][0], str , 5);
    }

    if (thismenu_sm0 == 0)//only at enter
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_units_fxKey);

        vscroll.init(db_mFuelSensor_irrTank_spTable_units, 2, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        //
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[UNITS OF NODE]"));
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_irrTank_spTable_units);
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_LENGTHUNITS_NUMITEM 1
#define MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM 11
const struct _fuelsensor_db_irregular_unitsOfTable_lengthUnits
{
    char name[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];
}
fuelsensor_db_irregular_unitsOfTable_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM] PROGMEM=
{
    "Centimeter",
};

inline static void fuelsensor_get_irregular_unitsOfTable_lengthUnits_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_irregular_unitsOfTable_lengthUnits[type].name );
}

static void mFuelSensor_irrTank_spTable_units_lengthUnits_printUnits(void)
{
    char buf[10];
    fuelsensor_get_irregular_unitsOfTable_lengthUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.length.units, buf);
    lcd.printAtPosc(1, 9, buf);
}

static inline void mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey0(void)
{
}
static inline void mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey1(void)
{
    //vscroll.sign_up();
}
static inline void mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey2(void)
{
    //vscroll.sign_down();
}
static inline void mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey3(void)
{
    //eth_app_datasend_stackPush(idx_fuelsensor_tank_irregular_spline_node_units_length_units);
    //eth_app.send.bf.request = 1;
}
static inline void mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_UNITS_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey0, mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey1, mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey2,
    mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey3, mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey4
};

void mFuelSensor_irrTank_spTable_units_lengthUnits_job(void)
{
    char buf[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];

    char db_mFuelSensor_irrTank_spTable_units_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM][LCDAN_STR_MAX_SIZE];

    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];

    for (uint8_t i=0; i<MENU_FUELSENSOR_LENGTHUNITS_NUMITEM; i++)
    {
        fuelsensor_get_irregular_unitsOfTable_lengthUnits_name(i, buf);
        //db_mFuelSensor_irrTank_spTable_units_lengthUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);
        itoa(i+1, buff, 10);
        strcpy(str, buff);
        strcat_P(str, PSTR("] "));
        strcat(str, buf);
        //
        lcdan_str_lineformat_align(&db_mFuelSensor_irrTank_spTable_units_lengthUnits[i][0], str , 2);
    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_units_lengthUnits_fxKey);

        vscroll.init(db_mFuelSensor_irrTank_spTable_units_lengthUnits, MENU_FUELSENSOR_LENGTHUNITS_NUMITEM, vscroll_dispShowBuff, 1, FEAT_MARK_ON);

        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[UNITS OF LENGTH]"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, 0, PSTR("Current: "));

        mFuelSensor_irrTank_spTable_units_lengthUnits_printUnits();
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_irrTank_spTable_units_lengthUnits);
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2, LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM 2
#define MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM 8
const struct _fuelsensor_db_irregular_unitsOfTable_volumenUnits
{
    char name[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];
}
fuelsensor_db_irregular_unitsOfTable_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM] PROGMEM=
{
    "Gallons",
    "Liters",
};

void fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_irregular_unitsOfTable_volumenUnits[type].name );
}
static void mFuelSensor_irrTank_spTable_units_volumenUnits_printVolume(void)
{
    char buf[10];
    fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units, buf);
    strcat(buf, " ");
    lcd.printAtPosc(1, 9, buf);//String(buf)+" "
}

static inline void mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey0(void)
{
}
static inline void mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey1(void)
{
    vscroll.sign_up();

}
static inline void mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units = posc;

    fuelsensor_savedata();

    mFuelSensor_irrTank_spTable_units_volumenUnits_printVolume();
    //
    eth_app_datasend_stackPush(idx_fuelsensor_tank_irregular_spline_node_units_volume_units);
    eth_app.send.bf.request = 1;
}
static inline void mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_UNITS_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey0, mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey1, mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey2,
    mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey3, mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey4
};

void mFuelSensor_irrTank_spTable_units_volumenUnits_job(void)
{
    char buf[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];

    char db_mFuelSensor_irrTank_spTable_units_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM][LCDAN_STR_MAX_SIZE];

    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];


    for (uint8_t i=0; i<MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM; i++)
    {
        fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(i, buf);
        //db_mFuelSensor_irrTank_spTable_units_volumenUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);

        itoa(i+1, buff, 10);
        strcpy(str, buff);
        strcat_P(str, PSTR("] "));
        strcat(str, buf);
        //
        lcdan_str_lineformat_align(&db_mFuelSensor_irrTank_spTable_units_volumenUnits[i][0], str , 2);

    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_units_volumenUnits_fxKey);
        //
        vscroll.init(db_mFuelSensor_irrTank_spTable_units_volumenUnits, MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM, vscroll_dispShowBuff, 2, FEAT_MARK_ON);
        //
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[UNITS OF VOLUME]"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, 0, PSTR("Current: "));

        mFuelSensor_irrTank_spTable_units_volumenUnits_printVolume();
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_irrTank_spTable_units_volumenUnits);
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}
