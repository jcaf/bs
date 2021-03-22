/****************************************************************************
* Project: Battery System
*
* 2016-05-24-15.52
* jcaf @ jcafOpenSuse
*
* Copyright 2013 Juan Carlos Aguero Flores
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

#include "menu_fuelSensor_calibration.h"
#include "setnum.h"

#include "leds.h"
#include "ethernet.h"
///////////////////////////////////////////////////////////////////////////////////////////////
//setnum definitions for this application
uint8_t mFuelSensor_lengthTankDepth_setnum_number_printxy_getFil(uint8_t selected)
{
    uint8_t vfil[3]= {LCD_ROW_1,LCD_ROW_2,LCD_ROW_3};
    return vfil[selected];
}
uint8_t mFuelSensor_lengthTankDepth_setnum_number_printxy_getCol(uint8_t selected)
{
    uint8_t vcol[3]= {13,13,13};
    return vcol[selected];
}

//manual
#define D1_SETNUM_CHARNUMBER_SELECTED 0//       #define SETNUM_PARAM_INNERTANK 0
#define D2_SETNUM_CHARNUMBER_SELECTED 1//        #define SETNUM_PARAM_ZERO2FULL  1
#define D3_SETNUM_CHARNUMBER_SELECTED 2//        #define SETNUM_PARAM_FULL2UPPER 2

#define D1_ASCHAR_NUMINTEGERS 3
#define D1_ASCHAR_NUMDECIMALS 2

#define D2_ASCHAR_NUMINTEGERS 3
#define D2_ASCHAR_NUMDECIMALS 2

#define D3_ASCHAR_NUMINTEGERS 3
#define D3_ASCHAR_NUMDECIMALS 2

//automatic
#define D1_ASCHAR_BUFFWITDH (D1_ASCHAR_NUMINTEGERS+1+D1_ASCHAR_NUMDECIMALS)
#define D1_ASCHAR_DIGPOS_DP D1_ASCHAR_NUMINTEGERS
#define D1_ASCHAR_DIGPOS_ENDOFDIGIT (D1_ASCHAR_NUMINTEGERS+1+D1_ASCHAR_NUMDECIMALS)-1

#define D2_ASCHAR_BUFFWITDH (D2_ASCHAR_NUMINTEGERS+1+D2_ASCHAR_NUMDECIMALS)
#define D2_ASCHAR_DIGPOS_DP D2_ASCHAR_NUMINTEGERS
#define D2_ASCHAR_DIGPOS_ENDOFDIGIT (D2_ASCHAR_NUMINTEGERS+1+D2_ASCHAR_NUMDECIMALS)-1

#define D3_ASCHAR_BUFFWITDH (D3_ASCHAR_NUMINTEGERS+1+D3_ASCHAR_NUMDECIMALS)
#define D3_ASCHAR_DIGPOS_DP D3_ASCHAR_NUMINTEGERS
#define D3_ASCHAR_DIGPOS_ENDOFDIGIT (D3_ASCHAR_NUMINTEGERS+1+D3_ASCHAR_NUMDECIMALS)-1
///////////////////////////////////////////////////////////////////////////////////////////////

static inline void mFuelSensor_calibration_fxKey0(void)
{
}
static inline void mFuelSensor_calibration_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mFuelSensor_calibration_fxKey2(void)
{
    vscroll.sign_down();
}

static inline void mFuelSensor_calibration_fxKey3(void)
{
    switch (vscroll.get_markPosc())
    {
        case 0:
            menu_setSubMenu(MGO_MFUELSENSOR_FULLZEROCALIBRATION_JOB);
            fuelsensor.bf.do_fullzero = FS_DO_FULL_CALIB;
            break;

        case 1:
            menu_setSubMenu(MGO_MFUELSENSOR_FULLZEROCALIBRATION_JOB);
            fuelsensor.bf.do_fullzero = FS_DO_ZERO_CALIB;
            break;

        case 2:
            //setnum initialization
            setnum_digit_reset_charnumber_reset();
            setnum_digit_blink_set_timeout(500);//ms
            setnum_charnumber_qty_of_charnumber(3);
            setnum_charnumber_thenumber_digitposition_init(D1_SETNUM_CHARNUMBER_SELECTED, D1_ASCHAR_DIGPOS_DP, D1_ASCHAR_DIGPOS_ENDOFDIGIT);
            setnum_charnumber_thenumber_digitposition_init(D2_SETNUM_CHARNUMBER_SELECTED, D2_ASCHAR_DIGPOS_DP, D2_ASCHAR_DIGPOS_ENDOFDIGIT);
            setnum_charnumber_thenumber_digitposition_init(D3_SETNUM_CHARNUMBER_SELECTED, D3_ASCHAR_DIGPOS_DP, D3_ASCHAR_DIGPOS_ENDOFDIGIT);
            setnum_printxy_register_pFil(mFuelSensor_lengthTankDepth_setnum_number_printxy_getFil);
            setnum_printxy_register_pCol(mFuelSensor_lengthTankDepth_setnum_number_printxy_getCol);
            //

            menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_JOB);
            break;
    }
    vscroll.stack_push();
    vscroll.base_offset_reset();
}

static inline void mFuelSensor_calibration_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MFUELSENSOR_MENU_JOB);
}

PTRFX_retVOID const mFuelSensor_calibration_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_calibration_fxKey0, mFuelSensor_calibration_fxKey1, mFuelSensor_calibration_fxKey2,
    mFuelSensor_calibration_fxKey3, mFuelSensor_calibration_fxKey4
};

void mFuelSensor_calibration_job(void)
{
#define MENU_FUELSENSOR_CALIBRATION 3

    char db_mFuelSensor_calibration[MENU_FUELSENSOR_CALIBRATION][LCDAN_STR_MAX_SIZE];

    lcdan_str_lineformat_align_P(&db_mFuelSensor_calibration[0][0],  PSTR("1]SET FULL SCALE"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_calibration[1][0],  PSTR("2]SET ZERO"), 0);
    lcdan_str_lineformat_align_P(&db_mFuelSensor_calibration[2][0],  PSTR("3]LENGTH TANK DEPTH"), 0);

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_calibration_fxKey);

        vscroll.init(db_mFuelSensor_calibration, MENU_FUELSENSOR_CALIBRATION, vscroll_dispShowBuff, 3, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[CALIBRATION]"));

        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mFuelSensor_calibration);
    vscroll.job();
    vscroll_lcd_show_disp();
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
inline static void mFuelSensor_FullZeroCalibration_escape(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_CALIBRATION_JOB);
    vscroll.stack_pop();
}

static void mFuelSensor_FullZeroCalibration_fxKey0(void)
{
    thismenu_sm1 = 0;

    if (thismenu_sm0 == 1)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
    }
    else if (thismenu_sm0 == 3)
    {
    }
    else if (thismenu_sm0 == 4)
    {
        mFuelSensor_FullZeroCalibration_escape();

    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)
    {
    }
}
static void mFuelSensor_FullZeroCalibration_fxKey1(void)
{
    thismenu_sm1 = 0;

    if (thismenu_sm0 == 1)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
    }
    else if (thismenu_sm0 == 3)
    {
    }
    else if (thismenu_sm0 == 4)
    {
        mFuelSensor_FullZeroCalibration_escape();

    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)
    {
        vscroll.sign_up();
    }
    else if (thismenu_sm0 == 7)
    {
        vscroll.sign_up();
    }

}
static void mFuelSensor_FullZeroCalibration_fxKey2(void)
{
    thismenu_sm1 = 0;

    if (thismenu_sm0 == 1)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)
    {
    }
    else if (thismenu_sm0 == 3)
    {
    }
    else if (thismenu_sm0 == 4)
    {
        mFuelSensor_FullZeroCalibration_escape();
    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)
    {
        vscroll.sign_down();
    }
    else if (thismenu_sm0 == 7)
    {
        vscroll.sign_down();
    }
}
static void mFuelSensor_FullZeroCalibration_fxKey3(void)
{
    uint8_t opt = vscroll.get_markPosc();
    thismenu_sm1 = 0;

    if (thismenu_sm0 == 1)//attention
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 2)//waiting
    {
        thismenu_sm0++;

        //
        if (fuelsensor.bf.do_fullzero == FS_DO_FULL_CALIB)
        {
            fuelsensor.request.cmd = FS_CMD_CODE_FULLRANGE_ADJ;
        }
        else if (fuelsensor.bf.do_fullzero == FS_DO_ZERO_CALIB)
        {
            fuelsensor.request.cmd = FS_CMD_CODE_ZERO_ADJ;
        }
        fuelsensor.request.bf.pending = 1;
        //
    }
    else if (thismenu_sm0 == 3)//do calib
    {
    }
    else if (thismenu_sm0 == 4)//success.
    {
        mFuelSensor_FullZeroCalibration_escape();
    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)//retry
    {
        if (opt == 0)
            thismenu_sm0 = 2;
        else
            mFuelSensor_FullZeroCalibration_escape();
    }
    else if (thismenu_sm0 == 7)//cancel calibration?
    {
        if (opt == 0)
        {
            digitalWrite(LED1, LOW);

            mFuelSensor_FullZeroCalibration_escape();
        }
        else
            thismenu_sm0 = thismenu_sm0_last;//restore last menu
    }

}
static void mFuelSensor_FullZeroCalibration_fxKey4(void)
{
    thismenu_sm1 = 0;

    if (thismenu_sm0 == 1)//ATTENTION warning
    {
        thismenu_sm0_last = thismenu_sm0;
        thismenu_sm0 = 7;//go escape
    }
    else if (thismenu_sm0 == 2)//"Press key <Enter> to")
    {
        thismenu_sm0_last = thismenu_sm0;
        thismenu_sm0 = 7;//go escape

        lcd.noBlink();
    }
    else if (thismenu_sm0 == 3)
    {

    }
    else if (thismenu_sm0 == 4)
    {
        mFuelSensor_FullZeroCalibration_escape();
    }
    else if (thismenu_sm0 == 5)
    {
        thismenu_sm1 = 0;
        thismenu_sm0++;
    }
    else if (thismenu_sm0 == 6)
    {
    }
    else if (thismenu_sm0 == 7)
    {
    }
}

PTRFX_retVOID const mFuelSensor_FullZeroCalibration_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_FullZeroCalibration_fxKey0, mFuelSensor_FullZeroCalibration_fxKey1, mFuelSensor_FullZeroCalibration_fxKey2,
    mFuelSensor_FullZeroCalibration_fxKey3, mFuelSensor_FullZeroCalibration_fxKey4
};

//////////////////////////////////////////////
void do_calib_disp_time(uint8_t do_fullzero, uint8_t _calib_disp_sec)
{
    static unsigned long millis_last;
    static unsigned long millis_last1;//add
    static uint8_t sm0, c,  counter_sec;
    //static uint8_t c_last;//x  full/zero display "progress-bar"

char str[LCDAN_STR_MAX_SIZE];
char buff[LCDAN_STR_MAX_SIZE];

    if (sm0 == 0)
    {
        counter_sec = 0;
        c=0;
        //c_last=0;
        //lcd.printAtPosc(2,16, "["+String(counter_sec)+"]" );

        strcpy_P(str, PSTR("["));
        itoa(counter_sec, buff, 10);
        strcat(str, buff);
        strcat_P(str, PSTR("]"));

        lcd.printAtPosc(2,16, str );

        millis_last = millis();
        millis_last1 = millis();//add
        sm0++;
    }
    else if (sm0 ==1)
    {
        if ( millis() - millis_last >= 1000)
        {
            millis_last = millis();
            //lcd.printAtPosc(2,16, "["+String(++counter_sec)+"]" );
             strcpy_P(str, PSTR("["));
            itoa(++counter_sec, buff, 10);
            strcat(str, buff);
            strcat_P(str, PSTR("]"));

            lcd.printAtPosc(2,16, str );

            //
            if (do_fullzero == FS_DO_FULL_CALIB)
            {
                ////x  full/zero display "progress-bar"
//                c+=1;
//                else
//                c+=4;
//                if (c<=20)
//                {
//                    for (uint8_t col = c_last; col<c; col++)//x  full/zero display "progress-bar"
//                    {
//                        lcd.setCursor(col,3);lcd.write(0xFF);
//                    }
//                    c_last = c;
//                }

                //
                if (c<20)
                {
                    lcd.setCursor(c,3);lcd.write(0xFF);
                    c++;
                }
                //

            }
        }

        if (do_fullzero == FS_DO_ZERO_CALIB)
        {
            if ( millis() - millis_last1 >= 250)// 5s/20car
            {
                millis_last1 = millis();
                //
                if (c<20)
                {
                    lcd.setCursor(c,3);lcd.write(0xFF);
                    c++;
                }
                //
            }
        }
    }

    if (_calib_disp_sec == 0)//abort
        sm0 = 0x00;
}
//////////////////////////////////////////////

void mFuelSensor_FullZeroCalibration_job(void)
{
    char msg[LCDAN_STR_MAX_SIZE];
    uint8_t _calib_disp_sec = 1;

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_FullZeroCalibration_fxKey);
        thismenu_sm0++;
        thismenu_sm1 = 0;
    }

    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();

            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[ATTENTION]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("You have to do Full"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("calibration firstly,"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("then Zero calib..."));

            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 == 2)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();

            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[MESSAGE]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Press key <Enter> to"));
            strcpy_P(msg, PSTR("begin "));

            if (fuelsensor.bf.do_fullzero == FS_DO_FULL_CALIB)
                strcat_P(msg,PSTR("Full"));
            else
                strcat_P(msg,PSTR("Zero"));

            sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, msg);
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("calibration...."));

            thismenu_sm1 = -1;
        }

        lcd.setCursor(17,4);
        lcd.blink();

        if ( (millis() - thismenu_last_millis0) >= 409)
        {
            thismenu_last_millis0 = millis();
            TogglingBit(PORTWxLED1,PINxLED1);
        }
        //-------------------------------------
    }

    if (thismenu_sm0 == 3)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.noBlink();
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[MESSAGE]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Doing calibration"));

            if (fuelsensor.bf.do_fullzero == FS_DO_FULL_CALIB)
                sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, 0, PSTR("Takes aprox.20s"));
            else
                sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, 0, PSTR("Takes aprox.5s"));

            digitalWrite(LED1, HIGH);

            thismenu_sm1 = -1;
        }

        if (fuelsensor.request.response_ext > FS_COD_RET_FRAME_INWAITING)//proseguir
        {
            if (fuelsensor.request.response_ext == FS_COD_RET_FRAME_CYCLE_TXRX_OK)
            {
                if (fuelsensor.bf.do_fullzero == FS_DO_FULL_CALIB)
                {
                    fuelsensor.fsEE.bf.full_calib = 1;
                    fuelsensor.fsEE.bf.zero_calib = 0;

                    fuelsensor_check_consistency();
                    fuelsensor_savedata();
                }
                else // FS_DO_ZERO_CALIB
                {
                    if (fuelsensor.fsEE.bf.full_calib == 1)
                    {
                        fuelsensor.fsEE.bf.zero_calib = 1;

                        fuelsensor_check_consistency();
                        fuelsensor_savedata();
                    }
                }
                thismenu_sm0 = 4;
            }
            else //Any type of error was produced
            {
                thismenu_sm0 = 5;
            }
            thismenu_sm1 = 0;

            digitalWrite(LED1, LOW);

            fuelsensor.request.response_ext = FS_COD_RET_CLEAR; //clear condition

             _calib_disp_sec = 0;//stop disp. seconds
        }

        do_calib_disp_time(fuelsensor.bf.do_fullzero, _calib_disp_sec);
    }

    if (thismenu_sm0 == 4)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[MESSAGE]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Calibration was"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("successfully!"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("(Press any key...)"));
            //delay(500);
            thismenu_sm1 = -1;
        }
    }
    if (thismenu_sm0 == 5)
    {
        if (thismenu_sm1 == 0)
        {
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[ERROR]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Calibration failed!"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Sensor need to be"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("re-calibrated!"));

            thismenu_sm1 = -1;
        }
    }

    if ( (thismenu_sm0 == 6) || (thismenu_sm0 == 7) )
    {
        char db_vscroll[2][LCDAN_STR_MAX_SIZE];
        lcdan_str_lineformat_align_P(&db_vscroll[0][0],  PSTR("1] Yes"), 7);
        lcdan_str_lineformat_align_P(&db_vscroll[1][0],  PSTR("2] No"), 7);

        if (thismenu_sm1 == 0)
        {
            //lcd.clear();

            if (thismenu_sm0 == 6)
            {
                sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[MESSAGE]"));
                sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Retry calibration?"));
            }
            else if (thismenu_sm0 == 7)
            {
                sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[MESSAGE]"));
                sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Cancel calibration?"));
            }
            vscroll.init(db_vscroll, 2, vscroll_dispShowBuff, 2, FEAT_MARK_ON, 0x00, 0x00);

            thismenu_sm1 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_2);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void lengthTankDepth_setnum_check_boundary(void);

static inline void mFuelSensor_lengthTankDepth_fxKey0(void)
{
}
static inline void mFuelSensor_lengthTankDepth_fxKey1(void)
{
    setnum_charnumber_thenumber_digitDec();
}

static inline void mFuelSensor_lengthTankDepth_fxKey2(void)
{
    setnum_charnumber_thenumber_digitInc();
}
static inline void mFuelSensor_lengthTankDepth_fxKey3(void)
{
    lengthTankDepth_setnum_check_boundary();
    setnum_charnumber_selectDigit_selectNumber();

    //help title
    uint8_t setnum_charnumber_currSelected = setnum_charnumber_get_currSelected();
    if (temp0_int8_t != setnum_charnumber_currSelected)
    {
        temp0_int8_t = setnum_charnumber_currSelected;

        if (setnum_charnumber_currSelected == D1_SETNUM_CHARNUMBER_SELECTED)
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[D1 in cm]"));
        else if (setnum_charnumber_currSelected == D2_SETNUM_CHARNUMBER_SELECTED)
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[D2 in cm]"));
        else if (setnum_charnumber_currSelected == D3_SETNUM_CHARNUMBER_SELECTED)
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[D3 in cm]"));
    }
}

static inline void mFuelSensor_lengthTankDepth_fxKey4(void)
{
    float innertank = setnum_charnumber_get_charvalue_asDou(D1_SETNUM_CHARNUMBER_SELECTED);
    float  zero2full = setnum_charnumber_get_charvalue_asDou(D2_SETNUM_CHARNUMBER_SELECTED);

    if ( ( innertank == 0.0f) || ( zero2full == 0.0f) )
    {
        menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_ERRORCONSISTENCY_JOB);
    }
    else
    {
        menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_NOERRORCONSISTENCY_JOB);
    }
}

PTRFX_retVOID const mFuelSensor_lengthTankDepth_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_lengthTankDepth_fxKey0, mFuelSensor_lengthTankDepth_fxKey1, mFuelSensor_lengthTankDepth_fxKey2,
    mFuelSensor_lengthTankDepth_fxKey3, mFuelSensor_lengthTankDepth_fxKey4
};

void mFuelSensor_lengthTankDepth_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_lengthTankDepth_fxKey);
        thismenu_sm0++;
    }
    if (thismenu_sm0 == 1)
    {
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(0, 0, PSTR("[D1 in cm]"));
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1, 0, PSTR("Inner Tank ="));//[HEIGHTS IN CM]
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(2, 0, PSTR("Zero->Full ="));//[EFFEC.LEN.SCAL.CAL]
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(3, 0, PSTR("Full->Upper="));//[LEN.TO UPPER SURF.]

        setnum_digit_reset_charnumber_reset();

        temp0_int8_t = setnum_charnumber_get_currSelected();

        setnum_charnumber_set_charvalue_asDou_aftFillZeros(D1_SETNUM_CHARNUMBER_SELECTED, fuelsensor.fsEE.calib.tank.innertank, D1_ASCHAR_BUFFWITDH, D1_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(D1_SETNUM_CHARNUMBER_SELECTED);

        setnum_charnumber_set_charvalue_asDou_aftFillZeros(D2_SETNUM_CHARNUMBER_SELECTED, fuelsensor.fsEE.calib.tank.zero2full, D2_ASCHAR_BUFFWITDH, D2_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(D2_SETNUM_CHARNUMBER_SELECTED);

        setnum_charnumber_set_charvalue_asDou_aftFillZeros(D3_SETNUM_CHARNUMBER_SELECTED, fuelsensor.fsEE.calib.tank.full2upper, D3_ASCHAR_BUFFWITDH, D3_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(D3_SETNUM_CHARNUMBER_SELECTED);

        thismenu_sm0 = -1;
    }
    setnum_digit_blinking();
}

static void lengthTankDepth_setnum_check_boundary(void)
{
    float D1 = setnum_charnumber_get_charvalue_asDou(D1_SETNUM_CHARNUMBER_SELECTED);
    float D2 = setnum_charnumber_get_charvalue_asDou(D2_SETNUM_CHARNUMBER_SELECTED);
    float D3 = setnum_charnumber_get_charvalue_asDou(D3_SETNUM_CHARNUMBER_SELECTED);

    if (D2 > D1)
    {
        D2 = D1;

        setnum_charnumber_set_charvalue_asDou_aftFillZeros(D2_SETNUM_CHARNUMBER_SELECTED, D2, D2_ASCHAR_BUFFWITDH, D2_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(D2_SETNUM_CHARNUMBER_SELECTED);
    }

    if ( (D2+D3) > D1)
    {
        D3 = D1-D2;

        setnum_charnumber_set_charvalue_asDou_aftFillZeros(D3_SETNUM_CHARNUMBER_SELECTED, D3, D3_ASCHAR_BUFFWITDH, D3_ASCHAR_NUMDECIMALS);
        setnum_charnumber_print_charvalue(D3_SETNUM_CHARNUMBER_SELECTED);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void mFuelSensor_lengthTankDepth_returnToItsCaller(void)
{
    menu_setSubMenu(MGO_MFUELSENSOR_CALIBRATION_JOB);
    vscroll.stack_pop();
}

static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey0(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
    }
    else if (thismenu_sm1 == 2)
    {
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey1(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
        vscroll.sign_up();
    }
    else if (thismenu_sm1 == 2)
    {
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey2(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
        vscroll.sign_down();
    }
    else if (thismenu_sm1 == 2)
    {
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}
static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey3(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
        uint8_t posc = vscroll.get_markPosc();

        //"[DATA INCONSISTENT]"
        //"Fix the errors?"
        if (posc == 0)
        {

            thismenu_sm0 = 1;

            fuelsensor.fsEE.calib.tank.innertank = setnum_charnumber_get_charvalue_asDou(D1_SETNUM_CHARNUMBER_SELECTED);
            fuelsensor.fsEE.calib.tank.zero2full = setnum_charnumber_get_charvalue_asDou(D2_SETNUM_CHARNUMBER_SELECTED);
            fuelsensor.fsEE.calib.tank.full2upper= setnum_charnumber_get_charvalue_asDou(D3_SETNUM_CHARNUMBER_SELECTED);

            menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_JOB);
        }
        else
        {
            fuelsensor.fsEE.bf.lengthsTankdepth_consistent = 0;

            //-------------- reset to default
            fuelsensor.fsEE.outputType.type = (FS_OUTPUTTYPE)LENGTH;
            fuelsensor.fsEE.outputType.length.units = FS_UNITMEA_LENGTH_PERCENTAGE;
            //--------------

            fuelsensor_savedata();

            thismenu_sm0 = 1;
            thismenu_sm1++;
        }
    }
    else if (thismenu_sm1 == 2)
    {
        //"[WARNING]"
        //"Data are wrong"
        //"to get volume"
        //"(Press any key...)"
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}

static void mFuelSensor_lengthTankDepth_errorConsistency_fxKey4(void)
{
    if (thismenu_sm1 == 0)
    {
        thismenu_sm1++;//skip the error
        thismenu_sm0 = 1;
    }
    else if (thismenu_sm1 == 1)
    {
    }
    else if (thismenu_sm1 == 2)
    {
        mFuelSensor_lengthTankDepth_returnToItsCaller();
    }
}

PTRFX_retVOID const mFuelSensor_lengthTankDepth_errorConsistency_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_lengthTankDepth_errorConsistency_fxKey0, mFuelSensor_lengthTankDepth_errorConsistency_fxKey1, mFuelSensor_lengthTankDepth_errorConsistency_fxKey2,
    mFuelSensor_lengthTankDepth_errorConsistency_fxKey3, mFuelSensor_lengthTankDepth_errorConsistency_fxKey4
};

void mFuelSensor_lengthTankDepth_errorConsistency_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_lengthTankDepth_errorConsistency_fxKey);
        thismenu_sm1 = 0x00;
        thismenu_sm0++;
    }

    if (thismenu_sm1 == 0)
    {
        if (thismenu_sm0 == 1)
        {
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[ERROR]"));

            float innertank = setnum_charnumber_get_charvalue_asDou(D1_SETNUM_CHARNUMBER_SELECTED);
            float zero2full = setnum_charnumber_get_charvalue_asDou(D2_SETNUM_CHARNUMBER_SELECTED);

            char msg[LCDAN_STR_MAX_SIZE];
            strcpy_P(msg, PSTR("Length "));

            if ( innertank == 0)
                strcat_P(msg, PSTR("[D1] "));

            if ( zero2full == 0)
                strcat_P(msg, PSTR("[D2]"));

            sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, msg);
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("must be greater"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("than zero"));

            thismenu_sm0 = -1;
        }
    }
    else if (thismenu_sm1 == 1)
    {
        char db_vscroll[2][LCDAN_STR_MAX_SIZE];

        lcdan_str_lineformat_align_P(&db_vscroll[0][0],  PSTR("1] Yes"), 7);
        lcdan_str_lineformat_align_P(&db_vscroll[1][0],  PSTR("2] No"), 7);

        if (thismenu_sm0 == 1)
        {
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[DATA INCONSISTENT]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Fix the errors?"));

            vscroll.init(db_vscroll, 2, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

            thismenu_sm0 = -1;
        }

        vscroll.set_db(db_vscroll);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_2);
    }
    else if (thismenu_sm1 == 2)
    {
        if (thismenu_sm0 == 1)
        {
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[WARNING]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Data are wrong"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("to get volume"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("(Press any key...)"));

            thismenu_sm0 = -1;
        }
    }
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey0(void)
{
}
static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey1(void)
{
    vscroll.sign_up();
}
static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey2(void)
{
    vscroll.sign_down();
}
static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    if (thismenu_sm0 == 1)
    {
        //[DATA IS CONSISTENT]
        //    "Save data?
        if (posc == 0)//Yes
        {
            //save data to EEPROM
            fuelsensor.fsEE.bf.lengthsTankdepth_consistent = 1;

            fuelsensor.fsEE.calib.tank.innertank = setnum_charnumber_get_charvalue_asDou(D1_SETNUM_CHARNUMBER_SELECTED);
            fuelsensor.fsEE.calib.tank.zero2full = setnum_charnumber_get_charvalue_asDou(D2_SETNUM_CHARNUMBER_SELECTED);
            fuelsensor.fsEE.calib.tank.full2upper= setnum_charnumber_get_charvalue_asDou(D3_SETNUM_CHARNUMBER_SELECTED);

            fuelsensor_savedata();

            //send changes to server
            eth_app_datasend_stackPush(idx_fuelsensor_calib_tank_innertank);
            eth_app_datasend_stackPush(idx_fuelsensor_calib_tank_zero2full);
            eth_app_datasend_stackPush(idx_fuelsensor_calib_tank_full2upper);
            eth_app.send.bf.request = 1;


        }
        // else//nothing to do for "NO" option
        // {
        // }

        thismenu_sm1 = 0;
        thismenu_sm0++;//next step
    }
    else if (thismenu_sm0 == 2)
    {
        //"[MESSAGE]"
        //"Exit ?"
        if (posc == 0)
        {
            mFuelSensor_lengthTankDepth_returnToItsCaller();
        }
        else
        {
            thismenu_sm0 = 0;//1;

            menu_setSubMenu(MGO_MFUELSENSOR_LENGTHTANKDEPTH_JOB);

        }
    }
}
static void mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey4(void)
{
}

PTRFX_retVOID const mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey0, mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey1, mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey2,
    mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey3, mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey4
};

void mFuelSensor_lengthTankDepth_noErrorConsistency_job(void)
{
    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mFuelSensor_lengthTankDepth_noErrorConsistency_fxKey);

        thismenu_sm1 = 0;

        thismenu_sm0++;
    }

    char db_vscroll[2][LCDAN_STR_MAX_SIZE];

    lcdan_str_lineformat_align_P(&db_vscroll[0][0],  PSTR("1] Yes"), 7);
    lcdan_str_lineformat_align_P(&db_vscroll[1][0],  PSTR("2] No"), 7);


    if (thismenu_sm0 == 1)
    {
        if (thismenu_sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);

            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[DATA IS CONSISTENT]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Save data?"));

            thismenu_sm1 = -1;
        }
    }
    else//thismenu_sm0 == 2
    {
        if (thismenu_sm1 == 0)
        {
            vscroll.init(db_vscroll, 2, vscroll_dispShowBuff, 2, FEAT_MARK_ON);

            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[MESSAGE]"));
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Exit ?"));//then EXIT

            thismenu_sm1 = -1;
        }
    }
    vscroll.set_db(db_vscroll);
    vscroll.job();
    vscroll_lcd_show_disp(LCD_ROW_2);
}
