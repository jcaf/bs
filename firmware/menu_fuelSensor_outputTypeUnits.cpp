/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.59
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

#include "menu_fuelSensor_outputTypeUnits.h"
#include "ethernet.h"
//__________________________________________________
#define DB_MENU_FUELSENSOR_OUTPUTTYPE_LENGTH 2

const struct _fuelsensor_db_outputType
{
    char name[10];
}
fuelsensor_db_outputType[DB_MENU_FUELSENSOR_OUTPUTTYPE_LENGTH] PROGMEM=
{
    "LENGTH",
    "VOLUME",
};

inline static void fuelsensor_get_outputType_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_outputType[type].name );
}
//__________________________________________________
//#define fuelsensor_get_outputType_name(type, buf) do{strcpy_P(buf, (char *) &fuelsensor_db_outputType[type].name );}while(0)

static void mFuelSensor_outputType_printOuputType(void)
{
    char buf[10];
    fuelsensor_get_outputType_name(fuelsensor.fsEE.outputType.type, buf);
    lcd.printAtPosc(1, 9, buf);
}
static inline void mFuelSensor_outputType_fxKey0(void)
{
}
static inline void mFuelSensor_outputType_fxKey1(void)
{
    if (fuelsensor.fsEE.bf.enable_get_volume)
        vscroll.sign_up();
}
static inline void mFuelSensor_outputType_fxKey2(void)
{
    if (fuelsensor.fsEE.bf.enable_get_volume)
        vscroll.sign_down();
}
static inline void mFuelSensor_outputType_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    fuelsensor.fsEE.outputType.type = (FS_OUTPUTTYPE) posc;

    fuelsensor_savedata();


    mFuelSensor_outputType_printOuputType();

    //send changes to server
    eth_app_datasend_stackPush(idx_fuelsensor_outputtype_type);
    eth_app.send.bf.request = 1;


    //
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (posc)
    {
        case 0:menu_setSubMenu(MGO_MFUELSENSOR_OUTPUTTYPE_LENGTHUNITS_JOB);//taskman_fuelSensor.sm2 = 1;//to units of length
        break;

        case 1:menu_setSubMenu(MGO_MFUELSENSOR_OUTPUTTYPE_VOLUMENUNITS_JOB);//taskman_fuelSensor.sm2 = 2;//to units of volume
        break;
    }
    //
}

static inline void mFuelSensor_outputType_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_MENU_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_outputType_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_outputType_fxKey0, mFuelSensor_outputType_fxKey1, mFuelSensor_outputType_fxKey2,
    mFuelSensor_outputType_fxKey3, mFuelSensor_outputType_fxKey4
};

//-----------------------------------------------------------------------------------------------------------
void mFuelSensor_outputType_job(void)
{
    char buf[10];

    char db_mFuelSensor_outputType[DB_MENU_FUELSENSOR_OUTPUTTYPE_LENGTH] [LCDAN_STR_MAX_SIZE];

    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];

    for (uint8_t i=0; i<DB_MENU_FUELSENSOR_OUTPUTTYPE_LENGTH; i++)
    {
        fuelsensor_get_outputType_name(i, buf);
        //db_mFuelSensor_outputType[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 5);
        itoa(i+1, buff, 10);
        strcpy(str, buff);
        strcat_P(str, PSTR("] "));
        strcat(str, buf);

        lcdan_str_lineformat_align(&db_mFuelSensor_outputType[i][0], str, 5);
    }

    if (thismenu_sm0 == 0)//only at enter
    {
        kb_change_keyDo_pgm(mFuelSensor_outputType_fxKey);

        vscroll.init(db_mFuelSensor_outputType, DB_MENU_FUELSENSOR_OUTPUTTYPE_LENGTH, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        //
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[OUTPUT TYPE]"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, 0, PSTR("Current: "));

        mFuelSensor_outputType_printOuputType();
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_outputType);
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_LENGTHUNITS_NUMITEM 3
#define MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM 15
const struct _fuelsensor_db_lengthUnits
{
    char name[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];
}
fuelsensor_db_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM] PROGMEM=
{
    "Percentage",
    "Centimeter",
    "Meter     ",
};

inline static void fuelsensor_get_lengthUnits_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_lengthUnits[type].name );
}

static void mFuelSensor_outputType_lengthUnits_printUnits(void)
{
    char buf[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];
    fuelsensor_get_lengthUnits_name(fuelsensor.fsEE.outputType.length.units, buf);
    lcd.printAtPosc(1, 9, buf);
}

static inline void mFuelSensor_outputType_lengthUnits_fxKey0(void)
{
}
static inline void mFuelSensor_outputType_lengthUnits_fxKey1(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.sign_up();
    }
}
static inline void mFuelSensor_outputType_lengthUnits_fxKey2(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        vscroll.sign_down();
    }
}
static inline void mFuelSensor_outputType_lengthUnits_fxKey3(void)
{
    if ((fuelsensor.fsEE.bf.lengthsTankdepth_consistent) && (fuelsensor.fsEE.bf.full_calib) && (fuelsensor.fsEE.bf.zero_calib))
    {
        uint8_t posc = vscroll.get_markPosc();

        fuelsensor.fsEE.outputType.length.units = posc;

        fuelsensor_savedata();

        mFuelSensor_outputType_lengthUnits_printUnits();

        //
        eth_app_datasend_stackPush(idx_fuelsensor_outputtype_length_units);
        eth_app.send.bf.request = 1;
    }
}
static inline void mFuelSensor_outputType_lengthUnits_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_OUTPUTTYPE_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_outputType_lengthUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_outputType_lengthUnits_fxKey0, mFuelSensor_outputType_lengthUnits_fxKey1, mFuelSensor_outputType_lengthUnits_fxKey2,
    mFuelSensor_outputType_lengthUnits_fxKey3, mFuelSensor_outputType_lengthUnits_fxKey4
};

//-----------------------------------------------------------------------------------------------------------

void mFuelSensor_outputType_lengthUnits_job(void)
{
    char buf[MENU_FUELSENSOR_LENGTHUNITS_LENGTHITEM];

    char db_mFuelSensor_lengthUnits[MENU_FUELSENSOR_LENGTHUNITS_NUMITEM][LCDAN_STR_MAX_SIZE];

    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];

    for (uint8_t i=0; i<MENU_FUELSENSOR_LENGTHUNITS_NUMITEM; i++)
    {
        fuelsensor_get_lengthUnits_name(i, buf);
        //db_mFuelSensor_lengthUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);

        itoa(i+1, buff, 10);
        strcpy(str, buff);
        strcat_P(str, PSTR("] "));
        strcat(str, buf);

        lcdan_str_lineformat_align(&db_mFuelSensor_lengthUnits[i][0], str, 2);
    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_outputType_lengthUnits_fxKey);

        vscroll.init(db_mFuelSensor_lengthUnits, MENU_FUELSENSOR_LENGTHUNITS_NUMITEM, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[UNITS OF LENGTH]"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, 0, PSTR("Current: "));

        mFuelSensor_outputType_lengthUnits_printUnits();
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_lengthUnits);
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM 2
#define MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM 8
const struct _fuelsensor_db_volumenUnits
{
    char name[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];
}
fuelsensor_db_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM] PROGMEM=
{
    "Gallons",
    "Liters ",
};

inline static void fuelsensor_get_volumenUnits_name(uint8_t type, char *buf)
{
    strcpy_P(buf, (char *) &fuelsensor_db_volumenUnits[type].name );
}
static void mFuelSensor_outputType_volumenUnits_printVolume(void)
{
    char buf[10];
    fuelsensor_get_volumenUnits_name(fuelsensor.fsEE.outputType.volume.units, buf);
    lcd.printAtPosc(1, 9, buf);
}


static inline void mFuelSensor_outputType_volumenUnits_fxKey0(void)
{
}
static inline void mFuelSensor_outputType_volumenUnits_fxKey1(void)
{
    vscroll.sign_up();

}
static inline void mFuelSensor_outputType_volumenUnits_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mFuelSensor_outputType_volumenUnits_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    fuelsensor.fsEE.outputType.volume.units = posc;

    fuelsensor_savedata();

    mFuelSensor_outputType_volumenUnits_printVolume();

    //
    eth_app_datasend_stackPush(idx_fuelsensor_outputtype_volume_units);
    eth_app.send.bf.request = 1;

}
static inline void mFuelSensor_outputType_volumenUnits_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_OUTPUTTYPE_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_outputType_volumenUnits_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_outputType_volumenUnits_fxKey0, mFuelSensor_outputType_volumenUnits_fxKey1, mFuelSensor_outputType_volumenUnits_fxKey2,
    mFuelSensor_outputType_volumenUnits_fxKey3, mFuelSensor_outputType_volumenUnits_fxKey4
};

//-----------------------------------------------------------------------------------------------------------

void mFuelSensor_outputType_volumenUnits_job(void)
{
    char buf[MENU_FUELSENSOR_VOLUMEUNITS_LENGTHITEM];

    char db_mFuelSensor_volumenUnits[MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM][LCDAN_STR_MAX_SIZE];

    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];


    for (uint8_t i=0; i<MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM; i++)
    {
        fuelsensor_get_volumenUnits_name(i, buf);
        //db_mFuelSensor_volumenUnits[i] = str_clearPrint(String(i+1)+ "] "+ String(buf), 2);

        itoa(i+1, buff, 10);
        strcpy(str, buff);
        strcat_P(str, PSTR("] "));
        strcat(str, buf);

        lcdan_str_lineformat_align(&db_mFuelSensor_volumenUnits[i][0], str, 2);
    }

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_outputType_volumenUnits_fxKey);

        vscroll.init(db_mFuelSensor_volumenUnits, MENU_FUELSENSOR_VOLUMEUNITS_NUMITEM, vscroll_dispShowBuff, 2, FEAT_MARK_ON);
        //
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[UNITS OF VOLUME]"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, 0, PSTR("Current: "));

        mFuelSensor_outputType_volumenUnits_printVolume();
        //
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_volumenUnits);
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}


