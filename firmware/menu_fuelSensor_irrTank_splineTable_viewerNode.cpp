/****************************************************************************
* Project: Battery System
*
* 2016-07-12-18.47
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
#include "ikey.h"
#include "kb.h"
#include "fuelsensor.h"
#include "menu.h"
#include "menu_mainMenu.h"
#include "menu_fuelSensor_irrTank_common.h"
#include "menu_fuelSensor_irrTank_splineTable_units.h"
#include "setnum.h"
#include "node.h"
#include "sysprint.h"
void mFuelSensor_irrTank_spTable_viewerNode_prev(void)
{
    if (--inode < 0 )
        inode = sptable_get_numberofNodes() -1;
}

void mFuelSensor_irrTank_spTable_viewerNode_next(void)
{
    if (++inode >= sptable_get_numberofNodes() )
        inode = 0;
}

void mFuelSensor_irrTank_spTable_viewerNode_header(const char *title)
{
    sysPrintLCDan_XY_wlineClr_wEOL3dots(0, 0, title);
    sysPrintLCDan_XY_wlineClr_wEOL3dots_P(2, 3, PSTR("[cm]  , "));
    char buf[10];
    fuelsensor_get_irregular_unitsOfTable_volumenUnits_name(fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units, buf);

    char str[SYSPRINT_STR_MAX_SIZE];
    strcpy_P(str, PSTR("["));
    strcat(str, buf);
    strcat_P(str, PSTR("]"));
    lcd.printAtPosc(2, 11, str);//"["+String(buf)+"]"

    lcd.printAtPosc(3, 9, ",");
}

void mFuelSensor_irrTank_spTable_viewerNode_node(int8_t inode)
{
    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];
    //
    strcpy_P(str, PSTR("NODE "));
    itoa(inode+1, buff, 10);
    strcat(str, buff);
    //
    sysPrintLCDan_XY_wlineClr_wEOL3dots(1, LCDAN_STR_FORMAT_ALIGN_CENTER, str);

    setnum_charnumber_set_charvalue_asDou_aftFillZeros(X_SETNUM_CHARNUMBER_SELECTED, sptable_get_node_X(inode), X_ASCHAR_BUFFWITDH, X_ASCHAR_NUMDECIMALS);
    setnum_charnumber_print_charvalue(X_SETNUM_CHARNUMBER_SELECTED);

    setnum_charnumber_set_charvalue_asDou_aftFillZeros(A_SETNUM_CHARNUMBER_SELECTED, sptable_get_node_A(inode), A_ASCHAR_BUFFWITDH, A_ASCHAR_NUMDECIMALS);
    setnum_charnumber_print_charvalue(A_SETNUM_CHARNUMBER_SELECTED);

}
