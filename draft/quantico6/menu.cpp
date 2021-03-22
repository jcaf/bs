/****************************************************************************
* Project: Battery System
*
* 2016-06-24-12.29
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
#include "ikey.h"
#include "kb.h"

#include "menu.h"
#include "menu_inputs.h"
#include "menu_fuelSensor.h"
#include "menu_fuelSensor_modelOfSensor.h"
#include "menu_fuelSensor_calibration.h"
#include "menu_fuelSensor_fuelTank.h"
#include "menu_fuelSensor_regularVolume.h"

#include "menu_fuelSensor_irrTank_menu.h"
#include "menu_fuelSensor_irrTank_common.h"
#include "menu_fuelSensor_irrTank_splineTable_units.h"
#include "menu_fuelSensor_irrTank_splineTable_new.h"
#include "menu_fuelSensor_irrTank_splineTable_modifNode.h"
#include "menu_fuelSensor_irrTank_splineTable_delNode.h"
//#include "menu_fuelSensor_irrTank_splineTable_viewerNode.h"
#include "menu_fuelSensor_irrTank_splineTable_insNode.h"


#include "menu_fuelSensor_outputTypeUnits.h"
//
#include "menu_bkground.h"
#include "menu_stackInDetail_V.h"
#include "menu_stackInDetail_E.h"

#include "menu_information.h"
#include "menu_contact.h"

#include "time.h"
#include "date.h"




//PTRFX_retVOID const mBkground[] PROGMEM=
PTRFX_retVOID mBkground[]=
{
mBkground_showStackSummary_job,
mBkground_insideStackSummary_job,
mBkground_stackInDetail_V_job,
mBkground_stackInDetail_E_job,
mBkground_stackInDetail_E_UnderVolt_job,
mBkground_stackInDetail_E_OverVolt_job,
mBkground_stackInDetail_E_OpenWire_job,
};

//PTRFX_retVOID const mInput[] PROGMEM=
PTRFX_retVOID mInput[]=
{
    mInput_menu_job,
    mInput_digitalInput_job,
    mInput_analogInput_job,
    mInput_fuelSensor_job
};

PTRFX_retVOID mInformation[]=
{
    mInformation_menu_job
};

PTRFX_retVOID mContact[]=
{
    mContact_menu_job
};

PTRFX_retVOID mSetTime[]=
{
    mSetTime_do,
    mSetTime_save_job
};

PTRFX_retVOID mSetDate[]=
{
    mSetDate_do,
    mSetDate_save_job
};


//PTRFX_retVOID const mFuelSensor[] PROGMEM=
PTRFX_retVOID mFuelSensor[]=
{
    mFuelSensor_warning_job,
	mFuelSensor_menu_job,
	//
	mFuelSensor_modelOfsensor_job,
	//
	mFuelSensor_calibration_job,
    mFuelSensor_FullZeroCalibration_job,
    mFuelSensor_lengthTankDepth_job,
    mFuelSensor_lengthTankDepth_errorConsistency_job,
    mFuelSensor_lengthTankDepth_noErrorConsistency_job,
    //
    mFuelSensor_tank_job,
    mFuelSensor_typeOfTank_job,
    mFuelSensor_rectangularVolume_job,
    mFuelSensor_rectangularVolumen_save_and_exit_job,

    //
    mFuelSensor_irrTank_menu_job,
    //
    mFuelSensor_irrTank_spTable_units_job,
    mFuelSensor_irrTank_spTable_units_lengthUnits_job,
    mFuelSensor_irrTank_spTable_units_volumenUnits_job,
    //
    mFuelSensor_irrTank_spTable_new_job,
    //
    mFuelSensor_irrTank_spTable_modifNode_viewerNode_job,
    mFuelSensor_irrTank_spTable_modifNode_job,
    //
    mFuelSensor_irrTank_spTable_delNode_viewerNode_job,

    mFuelSensor_irrTank_spTable_insNode_viewerNode_job,
    mFuelSensor_irrTank_spTable_insNode_job,
	//
	mFuelSensor_outputType_job,
    mFuelSensor_outputType_lengthUnits_job,
    mFuelSensor_outputType_volumenUnits_job,
};

PTRFX_retVOID *menu_v[MAIN_MENU_NUMMAX+1]=
//const PTRFX_retVOID *const menu_v[MAIN_MENU_NUMMAX+1] PROGMEM=
{
    mBkground,
    //
	mInput,
	(PTRFX_retVOID *)0,//outputs
	(PTRFX_retVOID *)0,//sys errors
	mInformation,//informatin
	mContact,//contatc
	mSetTime,
	mSetDate,
	mFuelSensor
};


int8_t thismenu_sm0;
int8_t thismenu_sm1;
int8_t thismenu_sm2;
int8_t thismenu_sm0_last;
unsigned long thismenu_last_millis0;
int8_t temp0_int8_t;

int8_t men=0;
int8_t men_last=men;

//int8_t submen=-1;//sub menu
int8_t submen=0;
int8_t submen_last = submen;

void menu_setMenu(int8_t m)
{
    men = m;
}
int8_t menu_getMenu(void)
{
    return men;
}

void menu_setSubMenu(int8_t m)
{
    submen = m;
}

int8_t menu_getSubMenu(void)
{
    return submen;
}

void menu_detect_changeMenu_changeSubMenu(void)
{
	if (men_last != men )
	{
		men_last = men;//a change has been produced
		thismenu_sm0 = 0;//reset
	}

	if (submen_last != submen )
	{
		submen_last =  submen;//a change has been produced
		thismenu_sm0 = 0;//reset
	}

}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
static inline void menu_generic_yesno_fxKey0(void)
{
}
static inline void menu_generic_yesno_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void menu_generic_yesno_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void menu_generic_yesno_fxKey3(void)
{
   if (vscroll.get_markPosc() == 0)//yes
        app_sm0 = MENU_RESP_YES;
    else
        app_sm0 = MENU_RESP_NO;
}
static inline void menu_generic_yesno_fxKey4(void)
{
}

PTRFX_retVOID const menu_generic_yesno_fxKey[KB_NUM_KEYS] PROGMEM =
{
    menu_generic_yesno_fxKey0, menu_generic_yesno_fxKey1, menu_generic_yesno_fxKey2,
    menu_generic_yesno_fxKey3, menu_generic_yesno_fxKey4
};

uint8_t menu_generic_yesno(uint8_t caption)
{
   	uint8_t cod_ret = MENU_RESP_WAITING;

   	#define DB_YESNO_NUM_OPTIONS 2
	String db_yesno[DB_YESNO_NUM_OPTIONS] =
	{
	    str_clearPrint("1] Yes", 7),
	    str_clearPrint("2] No", 7),
	};

	if (app_sm0 == 0)
	{
	     kb_change_keyDo_pgm(menu_generic_yesno_fxKey);

	    vscroll.init(db_yesno, DB_YESNO_NUM_OPTIONS, dispShowBuff, 2, FEAT_MARK_ON, 0x00, 0x00);
	    sysPrint_lcdPrintPSTR(0,STR_CENTER, PSTR("[MESSAGE]"));

	    if (caption == MGENERIC_CAPTION_DELETE)
	    	sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Delete node?"));
	    else if (caption == MGENERIC_CAPTION_EXIT)
			sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Exit?"));
		else if (caption == MGENERIC_CAPTION_SAVE_NEW_NODE)
			sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Save new node?"));
		else if (caption == MGENERIC_CAPTION_ENTER_NEW_NODE)
			sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Enter new node?"));
		else if (caption == MGENERIC_CAPTION_SAVE_CHANGES)
			sysPrint_lcdPrintPSTR(1,STR_CENTER, PSTR("Save changes?"));


	    app_sm0 = -1;
	}

	vscroll.set_db(db_yesno);
	vscroll.job();
	lcd_show_disp(LCD_ROW_2);

	//directed from keys
	if (app_sm0 == 1)
	{
	    cod_ret = MENU_RESP_YES;
	    app_sm0 = 0x00;
	}
	if (app_sm0 == 2)
	{
	    cod_ret = MENU_RESP_NO;
	    app_sm0 = 0x00;
	}
	return cod_ret;
}
