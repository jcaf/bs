/****************************************************************************
* Project: Battery System
*
* 2016-11-07-20.27
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
//#include "logf_class.h"
#include "logf.h"
#include "menu.h"
#include "logf_menu_common.h"
//#include "setnum.h"
#include "piz_class.h"
#include "piz.h"
#include <EEPROM.h>
#include "myEEPROM.h"
#include "menu_logf_editFx.h"
#include "mLogf_logeditor.h"

int8_t mLogf_editFx_options(int8_t *opt_dest);

///////////////////////////////////////////////////////////////////////////////////////////
//static inline void mLogf_editFx_fxKey0(void)
//{
//}
//static inline void mLogf_editFx_fxKey1(void)
//{
//}
//static inline void mLogf_editFx_fxKey2(void)
//{
//}
//static inline void mLogf_editFx_fxKey3(void)
//{
//}
//static inline void mLogf_editFx_fxKey4(void)
//{
//    vscroll.stack_pop();
//    menu_setSubMenu(MGO_MLOGF_MAIN_JOB);
//}
//PTRFX_retVOID const mLogf_editFx_fxKey[KB_NUM_KEYS] PROGMEM =
//{
//    mLogf_editFx_fxKey0, mLogf_editFx_fxKey1, mLogf_editFx_fxKey2, mLogf_editFx_fxKey3, mLogf_editFx_fxKey4
//};
uint8_t pre_modify_flag;
void pre_modify(uint8_t logexp_num)
{
    EEPROM.get(EEPROM_ADDR_LOGF_LOGEXP_ASSIGOUT_DATA + (sizeof(struct _logf_logExp_assigOut_data) )*(logexp_num),
               logf_logExp_assigOut_data);//ya esta cargado toda la data para ser manipulada

    ///////////////////////////////////
    piz.builder_content();

    if (piz.prop.numlines > 0)
    {
        if (piz.prop.numlines > 3)
        {
            piz.vp.numline = 3;
        }
        else
        {
            piz.vp.numline = piz.prop.numlines;
        }
        piz.vp.pvscroll->set_dispShowBuff_numline(piz.vp.numline);
        piz.vp.pvscroll->set_db_dbsize(piz.prop.numlines);
        piz.vp.pvscroll->set_db_base(0x00);
        piz.vp.pvscroll->set_db_offset(0x00);
    }

    //falta integrar con una accion para que se muestre en display
    piz.vp.pvscroll->job();
    vscroll_lcd_show_disp(LCD_ROW_1, (LCD_ROW_1+ piz.vp.numline-1) );

//    Symbol.setptr(0x00);
//    logf_outAssign.setptr(0x00);
//    piz.el.setptr(0x00);
}

void mLogf_editFx_job(void)
{
    int8_t menu_gui_event;
    int8_t opt_dest;
    char buff[4];
    char str[LCDAN_STR_MAX_SIZE];
    //
    static uint8_t logexp_num;

    if (thismenu_sm0 == 0)
    {
        thismenu_sm0++;
        thismenu_sm1 = 0;

        if ( num_exp_saved > 0 )
        {
            thismenu_sm0 = 1;
        }
        else
        {
            thismenu_sm0 = 2;//message DON'T EXIST ANY LOG.EXP
        }
    }
    if (thismenu_sm0 == 1)
    {
        menu_gui_event = mLogf_common_select_logexp_job(&opt_dest);//1- SELECT EXPRESION
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {

                //                EEPROM.get(EEPROM_ADDR_LOGF_LOGEXP_ASSIGOUT_DATA + (sizeof(struct _logf_logExp_assigOut_data) )*(logexp),
                //                logf_logExp_assigOut_data);//ya esta cargado toda la data para ser manipulada

                logexp_num = opt_dest;
                thismenu_sm0 = 3;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                thismenu_sm0 = 8;//exit
            }
        }
    }
    if (thismenu_sm0 == 2)//message DON'T EXIST ANY LOG.EXP
    {
        thismenu_sm0 = 8;//exit //por ahora solo EXIT!

        //        menu_gui_event = mLogf_common_select_logexp_job(&opt_dest);
        //        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        //        {
        //            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
        //            {
        //                //thismenu_sm0 = ;
        //            }
        //            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
        //            {
        //                thismenu_sm0 = ;//exit
        //            }
        //        }
    }
    if (thismenu_sm0 == 3)
    {
        menu_gui_event = mLogf_editFx_options(&opt_dest);//2- OPTIONS OVER SELECTED EXP
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == 0)
                {
                    thismenu_sm0 = 4;   //VIEW LOG.EXP
                }
                else if (opt_dest == 1)
                {
                    pre_modify_flag = 1;
                    thismenu_sm0 = 5;   //MODIFY LOG.EXP
                }
else
{
   thismenu_sm0 = 1;//deshabilitando las opciones de borrado
}
//                else if (opt_dest == 2)
//                    thismenu_sm0 = 6;
//                else if (opt_dest == 3)
//                    thismenu_sm0 = 7;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                thismenu_sm0 = 1;
            }
        }
    }

    if (thismenu_sm0 == 4)//view ---> EL ENTER EN QUE VA A TERMINAR????
    {
        menu_gui_event = mLogf_common_view_logExp_job(logexp_num);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                thismenu_sm0 = 0;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                thismenu_sm0 = 0;
            }
        }
    }
    if (thismenu_sm0 == 5)//modify
    {
        menu_gui_event = mLogf_logeditor_job(PSTR("Modify log.Exp"), logexp_num +1);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {

                //save in eeprom
                EEPROM.put(EEPROM_ADDR_LOGF_LOGEXP_ASSIGOUT_DATA + (sizeof(struct _logf_logExp_assigOut_data) )*(logexp_num),
                           logf_logExp_assigOut_data );

                for (uint8_t i=0; i< NUM_RELE_MAX; i++)
                {
                    EEPROM.put(EEPROM_ADDR_RELE_BUSY + ( sizeof(struct _logf_rele_bf) * i ) , logf_rele[i].bf );
                }

            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                for (uint8_t i=0; i<NUM_RELE_MAX; i++)//dejar intacto los flags de rele busy
                {
                    EEPROM.get(EEPROM_ADDR_RELE_BUSY + ( sizeof(struct _logf_rele_bf) * i ) , logf_rele[i].bf );
                }
            }
            thismenu_sm0 = 0x000000;//porq tomaria en cuenta si se ya no hay registros->mostrar "warning"
        }
    }
    //    if (thismenu_sm0 == 6)
    //    {
    //        menu_gui_event = mLogf_common_view_logExp_job(&opt_dest, logexp_num);
    //        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
    //        {
    //            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
    //            {
    //                //thismenu_sm0 = ;
    //            }
    //            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
    //            {
    //                //thismenu_sm0 = 3;//exit
    //            }
    //        }
    //    }
    //    if (thismenu_sm0 == 7)
    //    {
    //        menu_gui_event = mLogf_common_view_logExp_job(&opt_dest, logexp_num);
    //        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
    //        {
    //            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
    //            {
    //                //thismenu_sm0 = ;
    //            }
    //            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
    //            {
    //                //thismenu_sm0 = 3;//exit
    //            }
    //        }
    //    }
    if (thismenu_sm0 == 8)
    {
        thismenu_sm0 = 0x00;
        //
        vscroll.stack_pop();
        menu_setSubMenu(MGO_MLOGF_MAIN_JOB);

    }
}
/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
int8_t mLogf_editFx_sm_opt_sel;
int8_t    mLogf_editFx_sm_sm0;
int8_t    mLogf_editFx_sm_sm1;
static inline void mLogf_editFx_options_fxKey0(void)
{
}
static inline void mLogf_editFx_options_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_editFx_options_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_editFx_options_fxKey3(void)
{
    mLogf_editFx_sm_opt_sel = vscroll.get_markPosc();
    mLogf_editFx_sm_sm0 = 2;
}
static inline void mLogf_editFx_options_fxKey4(void)
{
    mLogf_editFx_sm_sm0 = 3;
}
PTRFX_retVOID const mLogf_editFx_options_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_editFx_options_fxKey0, mLogf_editFx_options_fxKey1, mLogf_editFx_options_fxKey2,
    mLogf_editFx_options_fxKey3, mLogf_editFx_options_fxKey4
};

int8_t mLogf_editFx_options(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;

    if (mLogf_editFx_sm_sm0 == 0)
    {
        mLogf_editFx_sm_sm0++;
        mLogf_editFx_sm_sm1 = 0;
    }
    if (mLogf_editFx_sm_sm0 == 1)
    {
        char db[4][LCDAN_STR_MAX_SIZE];
        lcdan_str_lineformat_align_P(&db[0][0],  PSTR("1] View"), 4);
        lcdan_str_lineformat_align_P(&db[1][0],  PSTR("2] Modify"), 4);
        lcdan_str_lineformat_align_P(&db[2][0],  PSTR("3] Delete"), 4);
        lcdan_str_lineformat_align_P(&db[3][0],  PSTR("4] Delete all"), 4);

        if (mLogf_editFx_sm_sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_editFx_options_fxKey);
            vscroll.init(db, 4, vscroll_dispShowBuff, 3, FEAT_MARK_ON, vscroll.base, vscroll.offset);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[SELECT]"));
            mLogf_editFx_sm_sm1 = -1;
        }
        vscroll.set_db(db);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (mLogf_editFx_sm_sm0 > 1)
    {
        if (mLogf_editFx_sm_sm0 == 2)
        {
            *opt_dest = mLogf_editFx_sm_opt_sel;
            cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        }

        else if (mLogf_editFx_sm_sm0 == 3)
        {
            cod_ret = MENU_GUI_EVENT_ESCAPE;
        }
        mLogf_editFx_sm_sm0 = 0;
    }
    return cod_ret;
}
