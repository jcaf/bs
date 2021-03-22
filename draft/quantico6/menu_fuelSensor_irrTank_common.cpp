/****************************************************************************
* Project: Battery System
*
* 2016-07-11-19.35
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
#include "fuelsensor.h"
#include "menu_fuelSensor_irrTank_common.h"
#include "setnum.h"
#include "node.h"

int8_t inode;


///////////////////////////////////////////////////////////////////////////////////////////////
//setnum definitions for this application
uint8_t mFuelSensor_irrTank_setnum_number_printxy_getFil(uint8_t selected)
{
    uint8_t vfil[2]= {3,3}; //2 number to print for this app
    return vfil[selected];
}
uint8_t mFuelSensor_irrTank_setnum_number_printxy_getCol(uint8_t selected)
{
    uint8_t vcol[2]= {2,12}; //2 number to print for this app
    return vcol[selected];
}
//all #defines in .h
///////////////////////////////////////////////////////////////////////////////////////////////

void splineTable_node_checkAndset_lowerLimit(int8_t inode)
{
    if (inode > 0) //add
    {
        double limit;
        double d_entered = setnum_charnumber_get_charvalue_asDou_fcurrSelected();
        uint8_t setnum_charnumber_currSelected = setnum_charnumber_get_currSelected();

        if (setnum_charnumber_currSelected == 0)//X
            limit = sptable_get_node_X(inode-1);
        else if (setnum_charnumber_currSelected == 1)//A
            limit = sptable_get_node_A(inode-1);

        if (d_entered < limit)
        {
            d_entered = limit;

            if (setnum_charnumber_currSelected == 0)//X
                setnum_charnumber_set_charvalue_asDou_aftFillZeros_fcurrSelected(d_entered, X_ASCHAR_BUFFWITDH, X_ASCHAR_NUMDECIMALS);
            else if (setnum_charnumber_currSelected == 1)//A
                setnum_charnumber_set_charvalue_asDou_aftFillZeros_fcurrSelected(d_entered, A_ASCHAR_BUFFWITDH, A_ASCHAR_NUMDECIMALS);

            setnum_charnumber_print_charvalue_fCurrSelected();
        }
    }
}

void splineTable_node_checkAndset_upperLimit(int8_t inode)//check to right
{
    for (int8_t i = inode; i < sptable_get_numberofNodes()-1; i++)//for (counter actual -> counter table)
    {
         if (sptable_get_node_X(i+1) <= sptable_get_node_X(i) )   //if ( next < actual)
            sptable_set_node_X(i+1, sptable_get_node_X(i)+1);//update

        if (sptable_get_node_A(i+1) <= sptable_get_node_A(i) )   //if ( next < actual)
            sptable_set_node_A(i+1, sptable_get_node_A(i)+1);//update
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
int8_t app_sm0;
int8_t app_sm1;

/////////////////////////////////////////////////////////////////////////////////////////////////
void spTable_disp_warning_minNode_fxKey0(void)
{
    app_sm1 = 0x00;
    app_sm0++;

}
void spTable_disp_warning_minNode_fxKey1(void)
{
    app_sm1 = 0x00;
    app_sm0++;
}
void spTable_disp_warning_minNode_fxKey2(void)
{
    app_sm1 = 0x00;
    app_sm0++;
}
void spTable_disp_warning_minNode_fxKey3(void)
{
    app_sm1 = 0x00;
    app_sm0++;
}
void spTable_disp_warning_minNode_fxKey4(void)
{
    app_sm1 = 0x00;
    app_sm0++;
}

PTRFX_retVOID const spTable_disp_warning_minNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    spTable_disp_warning_minNode_fxKey0, spTable_disp_warning_minNode_fxKey1, spTable_disp_warning_minNode_fxKey2,
    spTable_disp_warning_minNode_fxKey3, spTable_disp_warning_minNode_fxKey4
};

uint8_t spTable_disp_warning_minNode(uint8_t node_operation)
{
    uint8_t cod_ret = 0;
    String msg;

    if (app_sm0 == 0)
    {
        kb_change_keyDo_pgm(spTable_disp_warning_minNode_fxKey);

        app_sm1= 0x00;
        app_sm0++;
    }

    if (app_sm0 == 1)
    {
        if (app_sm1 == 0)
        {
            lcd.clear();
            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[WARNING]"));
            sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("The node-table needs"));
            sysPrint_lcdPrint(LCD_ROW_2, STR_CENTER, "at least " +String(SPLINE_NODES_MIN) + String(" nodes"));
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("(Press any key...)"));

            app_sm1 = -1;
        }
    }

    if (app_sm0 == 2)
    {
        if (app_sm1==0)
        {
            //
            msg = "If ";
            if (node_operation == MGENERIC_CAPTION_DELETE)
                msg+="delete,";
            else if (node_operation == MGENERIC_CAPTION_EXIT)
                msg+="exit, ";
            msg+="node-table";
            //

            sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("[WARNING]"));
            sysPrint_lcdPrint(LCD_ROW_1, STR_CENTER, msg);
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("can't be used in the"));//can't be used in the calc.
            sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("calc.(Press any key)"));//

            app_sm1 = -1;
        }
    }
    if (app_sm0 == 3)//actioned by keys
    {
        cod_ret = 1;
        app_sm0 = 0x00;//auto-reset app_sm0;
    }
    return cod_ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void spTable_disp_warning_maxNode_fxKey0(void)
{
    app_sm0 = 1;
}
void spTable_disp_warning_maxNode_fxKey1(void)
{
    app_sm0 = 1;
}
void spTable_disp_warning_maxNode_fxKey2(void)
{
    app_sm0 = 1;
}
void spTable_disp_warning_maxNode_fxKey3(void)
{
    app_sm0 = 1;
}
void spTable_disp_warning_maxNode_fxKey4(void)
{
    app_sm0 = 1;
}

PTRFX_retVOID const spTable_disp_warning_maxNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    spTable_disp_warning_maxNode_fxKey0, spTable_disp_warning_maxNode_fxKey1, spTable_disp_warning_maxNode_fxKey2,
    spTable_disp_warning_maxNode_fxKey3, spTable_disp_warning_maxNode_fxKey4
};

uint8_t spTable_disp_warning_maxNode(void)
{
    uint8_t cod_ret = 0;

    if (app_sm0 == 0)
    {
        kb_change_keyDo_pgm(spTable_disp_warning_maxNode_fxKey);

        sysPrint_lcdPrintPSTR(LCD_ROW_0, STR_CENTER, PSTR("Node-table has"));
        sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("reached maximum"));
        sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("size. Press any"));
        sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("key to exit..."));

        app_sm0 = -1;

    }
    if (app_sm0 == 1)//actioned by keys
    {
        cod_ret = 1;
        app_sm0 = 0x00;//auto-reset app_sm0;
    }
    return cod_ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
void spTable_disp_error_nodeEqualZero_fxKey0(void)
{
    app_sm0 = 1;
}
void spTable_disp_error_nodeEqualZero_fxKey1(void)
{
    app_sm0 = 1;
}
void spTable_disp_error_nodeEqualZero_fxKey2(void)
{
    app_sm0 = 1;
}
void spTable_disp_error_nodeEqualZero_fxKey3(void)
{
    app_sm0 = 1;
}
void spTable_disp_error_nodeEqualZero_fxKey4(void)
{
    app_sm0 = 1;
}

PTRFX_retVOID const spTable_disp_error_nodeEqualZero_fxKey[KB_NUM_KEYS] PROGMEM =
{
    spTable_disp_error_nodeEqualZero_fxKey0, spTable_disp_error_nodeEqualZero_fxKey1, spTable_disp_error_nodeEqualZero_fxKey2,
    spTable_disp_error_nodeEqualZero_fxKey3, spTable_disp_error_nodeEqualZero_fxKey4
};

uint8_t spTable_disp_error_nodeEqualZero(void)
{
    uint8_t cod_ret = 0;

    if (app_sm0 == 0)
    {
        kb_change_keyDo_pgm(spTable_disp_error_nodeEqualZero_fxKey);

        sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Length & Volume"));
        sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("must be greater"));
        sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("than zero..."));

        app_sm0 = -1;
    }
    if (app_sm0 == 1)//actioned by keys
    {
        cod_ret = 1;
        app_sm0 = 0x00;//auto-reset app_sm0;
    }

    return cod_ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void spTable_disp_error_nodeEqualPrevNode_fxKey0(void)
{
    app_sm0 = 1;
}
void spTable_disp_error_nodeEqualPrevNode_fxKey1(void)
{
    app_sm0 = 1;
}
void spTable_disp_error_nodeEqualPrevNode_fxKey2(void)
{
    app_sm0 = 1;
}
void spTable_disp_error_nodeEqualPrevNode_fxKey3(void)
{
    app_sm0 = 1;
}
void spTable_disp_error_nodeEqualPrevNode_fxKey4(void)
{
    app_sm0 = 1;
}

PTRFX_retVOID const spTable_disp_error_nodeEqualPrevNode_fxKey[KB_NUM_KEYS] PROGMEM =
{
    spTable_disp_error_nodeEqualPrevNode_fxKey0, spTable_disp_error_nodeEqualPrevNode_fxKey1, spTable_disp_error_nodeEqualPrevNode_fxKey2,
    spTable_disp_error_nodeEqualPrevNode_fxKey3, spTable_disp_error_nodeEqualPrevNode_fxKey4
};

uint8_t spTable_disp_error_nodeEqualPrevNode(void)
{
    uint8_t cod_ret = 0;

    if (app_sm0 == 0)
    {
        kb_change_keyDo_pgm(spTable_disp_error_nodeEqualPrevNode_fxKey);

        sysPrint_lcdPrintPSTR(LCD_ROW_1, STR_CENTER, PSTR("Length & Volume must"));
        sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("be greater than"));
        sysPrint_lcdPrintPSTR(LCD_ROW_3, STR_CENTER, PSTR("previous node..."));

        app_sm0 = -1;
    }
    if (app_sm0 == 1)//actioned by keys
    {
        cod_ret = 1;
        app_sm0 = 0x00;//auto-reset app_sm0;
    }
    return cod_ret;
}

