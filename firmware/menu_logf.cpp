/****************************************************************************
* Project: Battery System
*
* 2016-09-12-22.55
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

static inline void mLogf_main_fxKey0(void)
{
}
static inline void mLogf_main_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_main_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mLogf_main_fxKey3(void)
{
    int8_t submen = vscroll.get_markPosc();

    if ( submen<3)
    {
        vscroll.stack_push();

        if (submen == 0)
        {
            menu_setSubMenu(MGO_MLOGF_CONFIG_JOB);
        }
        else if (submen == 1)
        {
            Symbol.clear();
            logf_outAssign.clear();
            //
            menu_setSubMenu(MGO_MLOGF_NEWFX_JOB);
        }
        else if (submen == 2)
        {
            menu_setSubMenu(MGO_MLOGF_EDITFX_JOB);

        }
        vscroll.base_offset_reset();
    }

//    switch (submen)
//    {
//        case 0:
//
//
//        break;
//
//        case 1:
//
//        break;
//
//        case 2:
//
//        break;
//        /*
//        case 3:
//            menu_setSubMenu(MGO_MLOGF_EXEMANAGER_JOB);
//
//        break;
//        case 4:
//            menu_setSubMenu(MGO_MLOGF_MONITORFX_JOB);
//        break;
//        */
//        default:break;
//    }


}
static inline void mLogf_main_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setMenu(MGO_MAIN_MENU);
}

PTRFX_retVOID const mLogf_main_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_main_fxKey0, mLogf_main_fxKey1, mLogf_main_fxKey2,
    mLogf_main_fxKey3, mLogf_main_fxKey4
};

void mLogf_main_job(void)
{
    char db_logf_main[5][LCDAN_STR_MAX_SIZE];

    lcdan_str_lineformat_align_P(&db_logf_main[0][0],  PSTR("1] Config logic"), 0);
    lcdan_str_lineformat_align_P(&db_logf_main[1][0],  PSTR("2] New Fx"), 0);
    lcdan_str_lineformat_align_P(&db_logf_main[2][0],  PSTR("3] Edit Fx"), 0);
    lcdan_str_lineformat_align_P(&db_logf_main[3][0],  PSTR("4] Execute manager"), 0);
    lcdan_str_lineformat_align_P(&db_logf_main[4][0],  PSTR("5] Monitor fx"), 0);


    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mLogf_main_fxKey);

        vscroll.init(db_logf_main, 5, vscroll_dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[LOGICAL FUNCTION]"));

        thismenu_sm0 = -1;
    }
    vscroll.set_db(db_logf_main);
    vscroll.job();
    vscroll_lcd_show_disp();
}
