/****************************************************************************
* Project: Battery System
*
* 2016-07-11-20.03
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
#include "menu_fuelSensor_irrTank_splineTable_units.h"
#include "menu_fuelSensor_irrTank_splineTable_viewerNode.h"
#include "node.h"

static inline void mFuelSensor_irrTank_spTable_new_fxKey0(void)
{
}
static inline void mFuelSensor_irrTank_spTable_new_fxKey1(void)
{
    setnum_charnumber_thenumber_digitDec();
}

static inline void mFuelSensor_irrTank_spTable_new_fxKey2(void)
{
    setnum_charnumber_thenumber_digitInc();
}

static inline void mFuelSensor_irrTank_spTable_new_fxKey3(void)
{
    splineTable_node_checkAndset_lowerLimit(inode);
    setnum_charnumber_selectDigit_selectNumber();
}

static inline void mFuelSensor_irrTank_spTable_new_fxKey4(void)
{
    splineTable_node_checkAndset_lowerLimit(inode);
    thismenu_sm0 = 4;
}

PTRFX_retVOID const mFuelSensor_irrTank_spTable_new_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_new_fxKey0, mFuelSensor_irrTank_spTable_new_fxKey1, mFuelSensor_irrTank_spTable_new_fxKey2,
    mFuelSensor_irrTank_spTable_new_fxKey3, mFuelSensor_irrTank_spTable_new_fxKey4
};

void mFuelSensor_irrTank_spTable_new_job(void)//New Table
{
    uint8_t cod_ret;
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_new_fxKey);
        thismenu_sm1 = 0x0;

        thismenu_sm0++;
    }
    if (thismenu_sm0 == 1)
    {
        lcd.clear();
        mFuelSensor_irrTank_spTable_viewerNode_header("[NEW NODE-TABLE]");
        thismenu_sm0++;
    }
    if (thismenu_sm0 == 2)
    {
        setnum_digit_reset_charnumber_reset();

        sysPrint_lcdPrint(1, STR_CENTER, "NODE "+String( inode+1));

        float X=0.0f;
        float A=0.0f;
        if (inode > 0)//Load previous node
        {
            X = sptable_get_node_X(inode-1);
            A = sptable_get_node_A(inode-1);
        }
        //
        setnum_charnumber_set_charvalue_asDou_aftFillZeros(X_SETNUM_CHARNUMBER_SELECTED, X, X_ASCHAR_BUFFWITDH, X_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(X_SETNUM_CHARNUMBER_SELECTED);

        setnum_charnumber_set_charvalue_asDou_aftFillZeros(A_SETNUM_CHARNUMBER_SELECTED, A, A_ASCHAR_BUFFWITDH, A_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(A_SETNUM_CHARNUMBER_SELECTED);
        //
        thismenu_sm0++;
    }
    if (thismenu_sm0 == 3)
    {
        setnum_digit_blinking();
    }

    //add
    if (thismenu_sm0 == 4)
    {
        thismenu_sm0 = 7;//free error

        float Xentered = setnum_charnumber_get_charvalue_asDou(X_SETNUM_CHARNUMBER_SELECTED);
        float Aentered = setnum_charnumber_get_charvalue_asDou(A_SETNUM_CHARNUMBER_SELECTED);
        if ( (Xentered == 0.0f) || (Aentered == 0.0f) )
        {
            thismenu_sm0 = 5;
        }
        else
        {
            if (inode > 0)
            {
                if ((Xentered == sptable_get_node_X(inode-1))||(Aentered == sptable_get_node_A(inode-1)))
                {
                    thismenu_sm0 = 6;
                }
            }
        }
    }

    //errors
    if (thismenu_sm0 == 5)
    {
        if (spTable_disp_error_nodeEqualZero())
            thismenu_sm0 = 9;
    }
    if (thismenu_sm0 == 6)
    {
        if (spTable_disp_error_nodeEqualPrevNode())
            thismenu_sm0 = 9;
    }

    //
    if (thismenu_sm0 == 7)
    {
        cod_ret = menu_generic_yesno(MGENERIC_CAPTION_SAVE_NEW_NODE);
        if (cod_ret != MENU_RESP_WAITING)
        {
            if (cod_ret == MENU_RESP_YES)
            {
                //use as memory, if inode< MIN NODE -> restore original value from EEPROM
                float Xentered = setnum_charnumber_get_charvalue_asDou(X_SETNUM_CHARNUMBER_SELECTED);
                float Aentered = setnum_charnumber_get_charvalue_asDou(A_SETNUM_CHARNUMBER_SELECTED);
                sptable_set_node(inode, Xentered, Aentered);
                //
                inode++;//leave incremented for next enter

                if (sptable_node_GTEQ_maxNode(inode))
                    thismenu_sm0 = 8;//table has reach the maximum node
                else
                    thismenu_sm0 = 9;//next question...
            }
            else if (cod_ret == MENU_RESP_NO)
            {
                thismenu_sm0 = 9;//next question...
            }
        }
    }

    if (thismenu_sm0 == 8)
    {
        if (spTable_disp_warning_maxNode())
        {
            thismenu_sm0 = 12;//automatically exit
        }
    }

    if (thismenu_sm0 == 9)
    {
        cod_ret = menu_generic_yesno(MGENERIC_CAPTION_ENTER_NEW_NODE);
        if (cod_ret != MENU_RESP_WAITING)
        {
            if (cod_ret == MENU_RESP_YES)
            {
                thismenu_sm0 = 0x00;//
            }
            else if (cod_ret == MENU_RESP_NO)
            {
                if (!sptable_node_GTEQ_minNode(inode))//>=min node?
                    thismenu_sm0 = 10;
                else
                    thismenu_sm0 = 11;
            }
        }
    }

    if (thismenu_sm0 == 10)
    {
        if (spTable_disp_warning_minNode(MGENERIC_CAPTION_EXIT) )
            thismenu_sm0 = 11;
    }

    if (thismenu_sm0 == 11)
    {
        cod_ret = menu_generic_yesno(MGENERIC_CAPTION_EXIT);
        if (cod_ret != MENU_RESP_WAITING)
        {
            if (cod_ret == MENU_RESP_YES)
            {
                thismenu_sm0  = 12;
            }
            else if (cod_ret == MENU_RESP_NO)
                thismenu_sm0 = 0x00;//
        }
    }


    if (thismenu_sm0 == 12)//Exit saving nodes
    {
        //-------------------------------------------------------------------------
        if (inode >= SPLINE_NODES_MIN)
        {
            sptable_set_numberofNodes(inode);
            //
            fuelsensor.fsEE.bf.irregular_spline_nodetable_consistent = 1;
            spline_poly_get_coeffs();
            //
            fuelsensor_check_consistency();
            fuelsensor_savedata();
        }
        else
        {
            fuelsensor_loaddata();//restore any changes over fuelsensor.fsEE.tank.irregular.spline.node[inode].X or A
        }
        //-------------------------------------------------------------------------

        menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_MENU_JOB);
        vscroll.stack_pop();
    }
}
