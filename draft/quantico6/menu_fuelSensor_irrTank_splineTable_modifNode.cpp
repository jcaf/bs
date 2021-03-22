/****************************************************************************
* Project: Battery System
*
* 2016-07-09-13.33
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
#include "fuelsensor.h"
#include "menu.h"
#include "menu_fuelSensor.h"
#include "menu_mainMenu.h"
#include "menu_fuelSensor.h"//x load EEPROM
#include "menu_fuelSensor_fuelTank.h"
#include "menu_fuelSensor_irrTank_common.h"
#include "menu_fuelSensor_irrTank_splineTable_modifNode.h"
#include "menu_fuelSensor_irrTank_splineTable_units.h"
#include "menu_fuelSensor_irrTank_splineTable_viewerNode.h"
#include "setnum.h"
#include "node.h"

void mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey0(void)
{
}
void mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey1(void)
{
    mFuelSensor_irrTank_spTable_viewerNode_prev();
    thismenu_sm1 = 0;//trigger a change
}

void mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey2(void)
{
    mFuelSensor_irrTank_spTable_viewerNode_next();
    thismenu_sm1 = 0;//trigger a change
}

void mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey3(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_MODIFNODE_JOB);
}

void mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey4(void)
{
    thismenu_sm0 = 2;
}
PTRFX_retVOID const mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey0,
    mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey1,
    mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey2,
    mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey3,
    mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey4,
};

void mFuelSensor_irrTank_spTable_modifNode_viewerNode_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_modifNode_viewerNode_fxKey);
        lcd.clear();
        mFuelSensor_irrTank_spTable_viewerNode_header("[MODIFY][SELECT] ");
        thismenu_sm0++;
        thismenu_sm1 = 0x0;
    }
    //
    if (thismenu_sm0 == 1)//trigger from keys
    {
        if (thismenu_sm1 == 0)
        {
            mFuelSensor_irrTank_spTable_viewerNode_node(inode);
            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 == 2)
    {
        uint8_t cod_ret = menu_generic_yesno(MGENERIC_CAPTION_EXIT);
        if (cod_ret != MENU_RESP_WAITING)
        {
            if (cod_ret == MENU_RESP_YES)
            {
                thismenu_sm0  = 3;//->exit
            }
            else if (cod_ret == MENU_RESP_NO)
                thismenu_sm0 = 0x00;//
        }
    }

    if (thismenu_sm0 == 3)//exit
    {
        menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_MENU_JOB);
        vscroll.stack_pop();
    }
}

///////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_irrTank_spTable_modifNode_fxKey0(void)
{
}
static inline void mFuelSensor_irrTank_spTable_modifNode_fxKey1(void)
{
    setnum_charnumber_thenumber_digitDec();
}

static inline void mFuelSensor_irrTank_spTable_modifNode_fxKey2(void)
{
    setnum_charnumber_thenumber_digitInc();
}
static inline void mFuelSensor_irrTank_spTable_modifNode_fxKey3(void)
{
    splineTable_node_checkAndset_lowerLimit(inode);
    setnum_charnumber_selectDigit_selectNumber();
}
static inline void mFuelSensor_irrTank_spTable_modifNode_fxKey4(void)
{
    splineTable_node_checkAndset_lowerLimit(inode);
    thismenu_sm0 = 2;
}

PTRFX_retVOID const mFuelSensor_irrTank_spTable_modifNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_modifNode_fxKey0, mFuelSensor_irrTank_spTable_modifNode_fxKey1, mFuelSensor_irrTank_spTable_modifNode_fxKey2,
    mFuelSensor_irrTank_spTable_modifNode_fxKey3, mFuelSensor_irrTank_spTable_modifNode_fxKey4
};

static void mFuelSensor_irrTank_spTable_modifNode_fill_spline_nodes(void);

void mFuelSensor_irrTank_spTable_modifNode_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_modifNode_fxKey);

        mFuelSensor_irrTank_spTable_viewerNode_header("[MODIFY NODE]");

        setnum_digit_reset_charnumber_reset();
        sysPrint_lcdPrint(1, STR_CENTER, "NODE "+String( inode+1));
        thismenu_sm0++;
    }
    if (thismenu_sm0 == 1)
    {
        setnum_digit_blinking();
    }
    //
    if (thismenu_sm0 == 2)
    {
        uint8_t cod_ret = menu_generic_yesno(MGENERIC_CAPTION_SAVE_CHANGES);
        if (cod_ret != MENU_RESP_WAITING)
        {
            if (cod_ret == MENU_RESP_YES)
            {
                float Xentered = setnum_charnumber_get_charvalue_asDou(X_SETNUM_CHARNUMBER_SELECTED);
                float Aentered = setnum_charnumber_get_charvalue_asDou(A_SETNUM_CHARNUMBER_SELECTED);
                sptable_set_node(inode, Xentered, Aentered);
                //
                splineTable_node_checkAndset_upperLimit(inode);
                //

                fuelsensor_savedata();
            }
            else if (cod_ret == MENU_RESP_NO)
            {
                //fuelsensor_loaddata();//restore all modified vectors A/X
            }
            menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_MODIFNODE_VIEWERNODE_JOB);
        }
    }
}
