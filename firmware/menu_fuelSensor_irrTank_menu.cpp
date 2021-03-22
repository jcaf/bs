/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.49
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
#include "menu_mainMenu.h"
#include "menu_fuelSensor_fuelTank.h"
#include "menu_fuelSensor_irrTank_common.h"
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"
#include "setnum.h"
#include "node.h"
#include "sysprint.h"
static inline void mFuelSensor_irrTank_menu_fxKey0(void)
{
}
static inline void mFuelSensor_irrTank_menu_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_irrTank_menu_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mFuelSensor_irrTank_menu_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0:
            menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_UNITS_JOB);
            break;

        case 1:
            inode = 0x00;
            menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_NEW_JOB);
            break;

        case 2:
            if (sptable_get_numberofNodes() > 0)
            {
                inode = 0x00;
                menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_MODIFNODE_VIEWERNODE_JOB);
            }
            break;

        case 3:
            if (sptable_get_numberofNodes() > 0)
            {
                inode = 0x00;
                menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_DELNODE_VIEWERNODE_JOB);
            }
            break;

        case 4:
            if (sptable_get_numberofNodes() > 0)
            {
                inode = 0x00;
                menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_INSNODE_VIEWERNODE_JOB);
            }
            break;
    }
}

static inline void mFuelSensor_irrTank_menu_fxKey4(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_TANK_JOB);
    vscroll.stack_pop();
}

PTRFX_retVOID const mFuelSensor_irrTank_menu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_menu_fxKey0, mFuelSensor_irrTank_menu_fxKey1, mFuelSensor_irrTank_menu_fxKey2,
    mFuelSensor_irrTank_menu_fxKey3, mFuelSensor_irrTank_menu_fxKey4
};

void mFuelSensor_irrTank_menu_job(void)
{
    #define MENU_FUELSENSOR_IRREGULAR_MAINMENU 5
    char db_mFuelSensor_irrTank_menu[MENU_FUELSENSOR_IRREGULAR_MAINMENU][LCDAN_STR_MAX_SIZE];

    lcdan_str_lineformat_align_P(&db_mFuelSensor_irrTank_menu[0][0],  PSTR("1] UNITS OF TABLE"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_irrTank_menu[1][0],  PSTR("2] NEW TABLE"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_irrTank_menu[2][0],  PSTR("3] MODIFY NODE"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_irrTank_menu[3][0],  PSTR("4] DELETE NODE"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_irrTank_menu[4][0],  PSTR("5] INSERT NODE"), 0);


    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_menu_fxKey);
        vscroll.init(db_mFuelSensor_irrTank_menu, MENU_FUELSENSOR_IRREGULAR_MAINMENU, vscroll_dispShowBuff, 3, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[NODE TABLE]"));

        //setnum initialization
        setnum_digit_reset_charnumber_reset();
        setnum_digit_blink_set_timeout(500);//ms
        setnum_charnumber_qty_of_charnumber(2);
        setnum_charnumber_thenumber_digitposition_init(X_SETNUM_CHARNUMBER_SELECTED, X_ASCHAR_DIGPOS_DP, X_ASCHAR_DIGPOS_ENDOFDIGIT);
        setnum_charnumber_thenumber_digitposition_init(A_SETNUM_CHARNUMBER_SELECTED, A_ASCHAR_DIGPOS_DP, A_ASCHAR_DIGPOS_ENDOFDIGIT);
        setnum_printxy_register_pFil(mFuelSensor_irrTank_setnum_number_printxy_getFil);
        setnum_printxy_register_pCol(mFuelSensor_irrTank_setnum_number_printxy_getCol);
        //
        thismenu_sm0 =-1;
    }
    vscroll.set_db(db_mFuelSensor_irrTank_menu);//x local db
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_1);
}
