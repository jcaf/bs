/****************************************************************************
* Project: Battery System
*
* 2016-07-12-16.11
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
#include "menu_fuelSensor_irrTank_splineTable_viewerNode.h"
#include "node.h"
#include "sysprint.h"

void mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey0(void)
{
}
void mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey1(void)
{
    mFuelSensor_irrTank_spTable_viewerNode_prev();
    thismenu_sm1 = 0;//trigger a change
}

void mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey2(void)
{
    mFuelSensor_irrTank_spTable_viewerNode_next();
    thismenu_sm1 = 0;//trigger a change
}

void mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey3(void)
{
    thismenu_sm0 = 3;
}

void mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey4(void)
{
    fuelsensor_savedata();//direct save
    thismenu_sm0 = 6;
}

PTRFX_retVOID const mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey0,
    mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey1,
    mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey2,
    mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey3,
    mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey4,
};

void mFuelSensor_irrTank_spTable_delNode_viewerNode_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_irrTank_spTable_delNode_viewerNode_fxKey);

        thismenu_sm1 = 0x0;
        thismenu_sm0++;// = -1;
    }
    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();
            mFuelSensor_irrTank_spTable_viewerNode_header("[DELETE][SELECT]");
            thismenu_sm0++;
        }
    }
    if (thismenu_sm0 == 2)
    {
        if (thismenu_sm1 == 0)//trigger from keys
        {
            mFuelSensor_irrTank_spTable_viewerNode_node(inode);
            thismenu_sm1 = -1;
        }
    }

    //Add
    if (thismenu_sm0 == 3)
    {
        if (sptable_numberofNodes_EQ_minNode())
            thismenu_sm0 = 4;
        else
            thismenu_sm0 = 5;
    }

    if (thismenu_sm0 == 4)
    {
        if (spTable_disp_warning_minNode(MGENERIC_CAPTION_DELETE) )
            thismenu_sm0 = 5;
    }

    if (thismenu_sm0 == 5)
    {
        int8_t opt_dest;
        int8_t cod_ret = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Delete node?"), &opt_dest);
        if (cod_ret!= MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (cod_ret == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == GENERIC_YESNO_SELECT_YES)
                {
                    int8_t numberofNodes = node_del(&inode);
                    if (numberofNodes > 0)
                    {
                        thismenu_sm0 = 0x00;
                    }
                    else//GENERIC_YESNO_SELECT_NO
                    {
                        thismenu_sm0 = 0x00;
                    }
                }
                else if (cod_ret == MENU_GUI_EVENT_ESCAPE)
                {
                    thismenu_sm0 = 0x00;

                }

            }
        }
    }

    if (thismenu_sm0 == 6)
    {
        int8_t opt_dest;
        int8_t cod_ret = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Exit?"), &opt_dest);
        if (cod_ret != MENU_GUI_EVENT_WAITING_ACTION)
        {

            if (cod_ret == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == GENERIC_YESNO_SELECT_YES)
                {
                    thismenu_sm0  = 7;//->exit
                }
                else//GENERIC_YESNO_SELECT_NO
                {
                    thismenu_sm0 = 0x00;//
                }
            }
            else if (cod_ret == MENU_GUI_EVENT_ESCAPE)
            {
                thismenu_sm0 = 0x00;//
            }
        }
    }

    if (thismenu_sm0 == 7)//exit
    {
        menu_setSubMenu(MGO_MFUELSENSOR_IRRTANK_MENU_JOB);
        vscroll.stack_pop();
    }
}
