/****************************************************************************
* Project: Battery System
*
* 2016-07-15-19.14
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
#include "menu_fuelSensor_irrTank_splineTable_insNode.h"
#include "menu_fuelSensor_irrTank_splineTable_units.h"
#include "menu_fuelSensor_irrTank_splineTable_viewerNode.h"
#include "setnum.h"
#include "node.h"

void mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey0(void)
{
}
void mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey1(void)
{
    mFuelSensor_irrTank_spTable_viewerNode_prev();
    thismenu_sm1 = 0;//trigger a change
}

void mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey2(void)
{
    mFuelSensor_irrTank_spTable_viewerNode_next();
    thismenu_sm1 = 0;//trigger a change
}

void mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey3(void)
{
    thismenu_sm0 = 2;
}

void mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey4(void)
{
    thismenu_sm0 = 4;

}
PTRFX_retVOID const mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey0,
    mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey1,
    mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey2,
    mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey3,
    mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey4,
};

void mFuelSensor_irrTank_spTable_insNode_viewerNode_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_insNode_viewerNode_fxKey);
        lcd.clear();
        mFuelSensor_irrTank_spTable_viewerNode_header("[INSERT][->]");
        thismenu_sm0++;
        thismenu_sm1 = 0x0;
    }
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
        if (sptable_numberofNodes_EQ_maxNode())
            thismenu_sm0 = 3;
        else
            menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_INSNODE_JOB);
    }

    if (thismenu_sm0 == 3)
    {
        if (spTable_disp_warning_maxNode())
        {
            thismenu_sm0 = 7;//->exit
        }
    }

    if (thismenu_sm0 == 4)
    {
        if (sptable_numberofNodes_GTEQ_minNode())
            thismenu_sm0 = 6;
        else
            thismenu_sm0 = 5;
    }

    if (thismenu_sm0 == 5)
    {
        if (spTable_disp_warning_minNode(MGENERIC_CAPTION_EXIT) )
            thismenu_sm0 = 6;
    }

    if (thismenu_sm0 == 6)
    {
        uint8_t cod_ret = menu_generic_yesno(MGENERIC_CAPTION_EXIT);
        if (cod_ret != MENU_RESP_WAITING)
        {
            if (cod_ret == MENU_RESP_YES)
            {
                thismenu_sm0  = 7;//->exit
            }
            else if (cod_ret == MENU_RESP_NO)
                thismenu_sm0 = 0x00;//
        }
    }

    if (thismenu_sm0 == 7)//exit
    {
        menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_MENU_JOB);
        vscroll.stack_pop();
    }
}

///////////////////////////////////////////////////////////////////////////////////
static inline void mFuelSensor_irrTank_spTable_insNode_fxKey0(void)
{
}
static inline void mFuelSensor_irrTank_spTable_insNode_fxKey1(void)
{
    setnum_charnumber_thenumber_digitDec();
}

static inline void mFuelSensor_irrTank_spTable_insNode_fxKey2(void)
{
    setnum_charnumber_thenumber_digitInc();
}
static inline void mFuelSensor_irrTank_spTable_insNode_fxKey3(void)
{
    splineTable_node_checkAndset_lowerLimit(inode);
    setnum_charnumber_selectDigit_selectNumber();
}
static inline void mFuelSensor_irrTank_spTable_insNode_fxKey4(void)
{
    splineTable_node_checkAndset_lowerLimit(inode);
    thismenu_sm0 = 2;
}

PTRFX_retVOID const mFuelSensor_irrTank_spTable_insNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_insNode_fxKey0, mFuelSensor_irrTank_spTable_insNode_fxKey1, mFuelSensor_irrTank_spTable_insNode_fxKey2,
    mFuelSensor_irrTank_spTable_insNode_fxKey3, mFuelSensor_irrTank_spTable_insNode_fxKey4
};

void mFuelSensor_irrTank_spTable_insNode_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_insNode_fxKey);

        mFuelSensor_irrTank_spTable_viewerNode_header("[NEW NODE]");

        setnum_digit_reset_charnumber_reset();

        inode++;//insert to right

        sysPrint_lcdPrint(1, STR_CENTER, "NODE "+String( inode+1));
        //
        float X;//=0;
        float A;//=0;
        if (inode > 0)//forever will be >0 because this insert to the right
        {
            X = sptable_get_node_X(inode-1);
            A = sptable_get_node_A(inode-1);
        }
        setnum_charnumber_set_charvalue_asDou_aftFillZeros(X_SETNUM_CHARNUMBER_SELECTED, X, X_ASCHAR_BUFFWITDH, X_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(X_SETNUM_CHARNUMBER_SELECTED);

        setnum_charnumber_set_charvalue_asDou_aftFillZeros(A_SETNUM_CHARNUMBER_SELECTED, A, A_ASCHAR_BUFFWITDH, A_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(A_SETNUM_CHARNUMBER_SELECTED);
        //
        thismenu_sm0++;
    }

    if (thismenu_sm0 == 1)
    {
        setnum_digit_blinking();
    }
    //
    if (thismenu_sm0 == 2)
    {
        uint8_t cod_ret = menu_generic_yesno(MGENERIC_CAPTION_SAVE_NEW_NODE);
        if (cod_ret != MENU_RESP_WAITING)
        {
            if (cod_ret == MENU_RESP_YES)
            {
                float Xentered = setnum_charnumber_get_charvalue_asDou(X_SETNUM_CHARNUMBER_SELECTED);
                float Aentered = setnum_charnumber_get_charvalue_asDou(A_SETNUM_CHARNUMBER_SELECTED);

                node_insert_rigth(inode, Xentered, Aentered);
                //
                splineTable_node_checkAndset_upperLimit(inode);
                //

                fuelsensor_savedata();
            }
            else if (cod_ret == MENU_RESP_NO)
            {
                //fuelsensor_loaddata();//restore all modified vectors A/X
                inode--;
            }
            menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_SPLINETABLE_INSNODE_VIEWERNODE_JOB);
        }
    }
}
