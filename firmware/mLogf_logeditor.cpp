/****************************************************************************
* Project: Battery System
*
* 2016-11-11-22.01
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
#include "logf_class.h"
#include "logf.h"
#include "menu.h"
#include "menu_logf_newFx.h"
#include "logf_menu_common.h"
#include "setnum.h"
#include "piz_class.h"
#include "piz.h"
#include <EEPROM.h>
#include "myEEPROM.h"

#include "menu_logf_editFx.h"//--> solo para el pre_modify

int8_t mLogf_logexp_enter_symbol_job(int8_t *opt_dest);
int8_t mLogf_edit_opt(int8_t *opt_dest);
int8_t mLogf_select_loginput_assign(int8_t *opt_dest);
int8_t mLogf_assign_job(int8_t *opt_dest);
int8_t mLogf_assigFx_functs_job(int8_t *opt_dest);

enum
{
    MLOGF_INIT = 0,
    MLOGF_SHOW_PIZ,
    MLOGF_ENTER_SELECT_METHOD,
    MLOGF_ENTER_EDITING,
    MLOGF_ENTER_LOGINPUT_OR_ASSIGN,
    MLOGF_ENTER_LOGINPUT,
    MLOGF_ENTER_ASSIGNMENT,
    //
    MLOGF_COMMON_DIGINPUT,
    MLOGF_COMMON_ANINPUT,
    MLOGF_COMMON_RELAY,
    MLOGF_COMMON_LOGICALOP,
    MLOGF_COMMON_RELATIONALOP,
    MLOGF_COMMON_ARITHMETICOP,
    MLOGF_COMMON_ASOCIATIVITYOP,
    MLOGF_COMMON_PUNCTUATIONMARKOP,
    MLOGF_COMMON_ASSIGNAMENTOP,
    MLOGF_COMMON_LITERAL,
    MLOGF_COMMON_VOLTAGE,
    MLOGF_COMMON_CURRENT,
    MLOGF_COMMON_TEMPERATURE,
    MLOGF_COMMON_FLAGS,
    //
    MLOGF_APPLY_SYMBOL_METHOD,
    //
    MLOGF_ENTER_ASSIGN_RELAY,
    MLOGF_ENTER_ASSIGN_FX,
    MLOGF_ENTER_ASSIGN_FX_TIMER,

    //
    MLOGF_APPLY_ASSIGN_METHOD,
    //
    MLOGF_ESCAPE,
    //
    MLOGF_QUESTION_EXIT,
    MLOGF_QUESTION_SAVE,
    MLOGF_EXIT_FROM_THIS_MENU,
};


///////////////////////////////////////////////////////////////////////////////////////////
enum _PIZ_MOV_PTR
{
    REVERSE = -1,
    FORWARD = +1,
};
void piz_move_ptr(int8_t el_type, int8_t dir)
{
    int8_t ptr;
    static int8_t symbol_top=0;
    static int8_t logf_outAssign_bottom=0;

    if (logf_outAssign.get_length() == 0)//only exist SYMBOL x default
    {
        ptr = Symbol.getptr();

        if (dir == _PIZ_MOV_PTR::FORWARD)
                Symbol.incptr();
        else
        {
           if (ptr > 0)
                Symbol.decptr();
        }
    }
    else//existe logf_outAssign
    {
        if  (el_type == _ELEMENT_TYPE::LOGF_SYMB)
        {
            ptr = Symbol.getptr();

            if (dir == _PIZ_MOV_PTR::REVERSE)
            {
                if (symbol_top == 1)
                    symbol_top = 0;
                else
                {
                    if (ptr > 0)
                        Symbol.decptr();
                }
            }
            else //dir == +1
            {
                if ( ptr < Symbol.get_length()-1 )
                    Symbol.incptr();
            }

            logf_outAssign_bottom = 1;
        }
        else if ((el_type == _ELEMENT_TYPE::ASSIGNMENT) ||
                 (el_type == _ELEMENT_TYPE::POINTER_TO_NEXT))//fix
        {
            ptr = logf_outAssign.getptr();

            if (dir == _PIZ_MOV_PTR::FORWARD)
            {
                if (logf_outAssign_bottom == 1)
                    logf_outAssign_bottom = 0;
                else
                    logf_outAssign.incptr();//can increment until "NEXT"
            }
            else //dir == -1
            {
                if ( ptr > 0 )
                    logf_outAssign.decptr();
            }

            symbol_top = 1;
        }

        //
        if ( Symbol.getptr() >= Symbol.get_length() )//apply fix
        {
            Symbol.setptr(Symbol.get_length()-1);//go back pointer
            //Fix
            symbol_top = 1;
        }
        //

    }
}
///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_logeditor_fxKey0(void)
{
}
static inline void mLogf_logeditor_fxKey1(void)
{
    if ( piz.el.getptr() > 0 )
    {
        piz.el.decptr();
        piz_move_ptr( piz.el.get_type(), _PIZ_MOV_PTR::REVERSE );
        //
        piz.action.type = PIZ_CURSOR_ACTION;
        piz.action.subtype = PIZ_CURSOR_ACTION_LEFT;
    }
}
static inline void mLogf_logeditor_fxKey2(void)
{
    if (piz.el.getptr() < piz.el.get_length()-1 )//length - POINTER_TO_NEXT
    {
        piz.el.incptr();
        piz_move_ptr( piz.el.get_type(), _PIZ_MOV_PTR::FORWARD );
        //
        piz.action.type = PIZ_CURSOR_ACTION;
        piz.action.subtype = PIZ_CURSOR_ACTION_RIGHT;
    }
}
static inline void mLogf_logeditor_fxKey3(void)
{
    //
    PIZ_VP_CONFIG piz_vp_lastconfig;
    piz_vp_lastconfig.base = piz.vp.pvscroll->get_db_base();
    piz_vp_lastconfig.offset = piz.vp.pvscroll->get_db_offset();
    piz.vp.set_lastconfig(piz_vp_lastconfig);
    //
    lcd.noBlink();
    gpmenu[0].sm0 = MLOGF_ENTER_SELECT_METHOD;
    //
    piz.action.type = PIZ_SYMBOL_ACTION;
    piz.action.subtype = PIZ_SYMBOL_ASSIGN_ACTION_NONE;
}
static inline void mLogf_logeditor_fxKey4(void)
{
    lcd.noBlink();
    gpmenu[0].sm0 = MLOGF_QUESTION_EXIT;
}
PTRFX_retVOID const mLogf_logeditor_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_logeditor_fxKey0, mLogf_logeditor_fxKey1, mLogf_logeditor_fxKey2, mLogf_logeditor_fxKey3, mLogf_logeditor_fxKey4
};

int8_t pre_modify_flag2;

int8_t mLogf_logeditor_job(const char *caption, uint8_t num_exp)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    int8_t menu_gui_event;
    int8_t opt_dest;
    char buff[4];
    char str[LCDAN_STR_MAX_SIZE];

    static int8_t piz_action_subtype;
    class _logf_symbol symbol;//temporal
    class _logf_output logf_output;//temporal
    float valf;

    if (gpmenu[0].sm0 == MLOGF_INIT)
    {
        gpmenu[0].sm0++;
        gpmenu[0].sm1 = 0;
    }
    if (gpmenu[0].sm0 == MLOGF_SHOW_PIZ)
    {
        if (gpmenu[0].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_logeditor_fxKey);
            //
            strcpy(str, "[");
            strcat_P(str, caption);
            strcat(str, "][");

            itoa(num_exp, buff, 10);
            strcat(str, buff);
            strcat(str, "]");
            //
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_0, 0, str);
//+++
if (pre_modify_flag == 1)
{
    pre_modify(num_exp-1);

Symbol.setptr(0x00);
logf_outAssign.setptr(0x00);
piz.el.setptr(0x00);

    pre_modify_flag = 0;
}

//if (pre_modify_flag2 == 1)
//{
//    pre_modify(num_exp-1);//simplemente volver "a pintar" el contenido
////no resetear los punteros,
////Symbol.setptr(0x00);
////logf_outAssign.setptr(0x00);
////piz.el.setptr(0x00);
//
//    pre_modify_flag2 = 0;
//}

//+++
            //no es percibido este set blink
            piz.cursor.set_f(LCD_ROW_1);
            piz.cursor.set_c(0);
            piz.cursor.blinkat_fc();
            //
            lcd.blink();

            gpmenu[0].sm1 = -1;
        }
        if (piz.action.type != PIZ_IDLE_ACTION)
        {
            piz_man();
            piz.action.type = PIZ_IDLE_ACTION;
            piz.action.subtype = PIZ_SYMBOL_ASSIGN_ACTION_NONE;
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_ENTER_SELECT_METHOD)
    {
        int8_t el_type = piz.el.get_type();
        if (el_type == _ELEMENT_TYPE::POINTER_TO_NEXT)
        {
            gpmenu[0].sm0 = MLOGF_ENTER_LOGINPUT_OR_ASSIGN;
            piz_action_subtype = PIZ_SYMBOL_ASSIGN_ACTION_NEW;
        }
        else
        {
            gpmenu[0].sm0 = MLOGF_ENTER_EDITING;//replace, insert, delete/all
        }

        piz.el.save( piz.el.getptr() );//save current element
    }

    if (gpmenu[0].sm0 == MLOGF_ENTER_EDITING)
    {
        menu_gui_event = mLogf_edit_opt(&opt_dest);//return with SYMBOL_REPLACE / SYMBOL_INSERT
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                piz_action_subtype = opt_dest;
                gpmenu[0].sm0 = MLOGF_ENTER_LOGINPUT_OR_ASSIGN;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;//exit
            }
        }
    }

    if (gpmenu[0].sm0 == MLOGF_ENTER_LOGINPUT_OR_ASSIGN)
    {
        int8_t loginput_assign_symb_operation;
        menu_gui_event = mLogf_select_loginput_assign(&loginput_assign_symb_operation);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (loginput_assign_symb_operation == 0)
                    gpmenu[0].sm0 = MLOGF_ENTER_LOGINPUT;
                else
                    gpmenu[0].sm0 = MLOGF_ENTER_ASSIGNMENT;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
                gpmenu[0].sm0 = MLOGF_ESCAPE;//exit
        }
    }

    if (gpmenu[0].sm0 == MLOGF_ENTER_LOGINPUT)
    {
        menu_gui_event = mLogf_logexp_enter_symbol_job(&opt_dest);
        if (menu_gui_event!= MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                switch (opt_dest)
                {
                    case 0:
                        gpmenu[0].sm0 = MLOGF_COMMON_DIGINPUT;
                        break;
                    case 1:
                        gpmenu[0].sm0 = MLOGF_COMMON_ANINPUT;
                        break;
                    case 2:
                        gpmenu[0].sm0 = MLOGF_COMMON_RELAY;
                        break;
                    case 3:
                        gpmenu[0].sm0 = MLOGF_COMMON_LOGICALOP;
                        break;
                    case 4:
                        gpmenu[0].sm0 = MLOGF_COMMON_RELATIONALOP;
                        break;
                    case 5:
                        gpmenu[0].sm0 = MLOGF_COMMON_ARITHMETICOP;
                        break;
                    case 6:
                        gpmenu[0].sm0 = MLOGF_COMMON_ASOCIATIVITYOP;
                        break;
                    case 7:
                        gpmenu[0].sm0 = MLOGF_COMMON_PUNCTUATIONMARKOP;
                        break;
                    case 8:
                        gpmenu[0].sm0 = MLOGF_COMMON_ASSIGNAMENTOP;
                        break;
                    case 9:
                        gpmenu[0].sm0 = MLOGF_COMMON_LITERAL;
                        break;
                    case 10:
                        gpmenu[0].sm0 = MLOGF_COMMON_VOLTAGE;
                        break;
                    case 11:
                        gpmenu[0].sm0 = MLOGF_COMMON_CURRENT;
                        break;
                    case 12:
                        gpmenu[0].sm0 = MLOGF_COMMON_TEMPERATURE;
                        break;
                    case 13:
                        gpmenu[0].sm0 = MLOGF_COMMON_FLAGS;
                        break;

                    default:
                        break;
                }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }

    if (gpmenu[0].sm0 == MLOGF_COMMON_DIGINPUT)
    {
        menu_gui_event = mLogf_common_digInput_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPRND_BOOLEAN);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_DIG_INPUT);
                symbol.set_valui(opt_dest);
                symbol.pval = (class _logf_digital_input *) &logf_digital_input[opt_dest];

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_ANINPUT)
    {
        menu_gui_event = mLogf_common_anInput_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPRND_FLOAT);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_AN_INPUT);
                symbol.set_valui(opt_dest);
                symbol.pval = (class _logf_analog_input *) &logf_analog_input[opt_dest];

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_RELAY)
    {
        menu_gui_event = mLogf_common_relay_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPRND_BOOLEAN);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_RELAY);
                symbol.set_valui(opt_dest);
                symbol.pval = (class _logf_rele *) &logf_rele[opt_dest];

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_LOGICALOP)
    {
        menu_gui_event = mLogf_common_logicalOP_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPERATOR);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_LOGICAL_OP);
                symbol.set_valui(opt_dest);

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_RELATIONALOP)
    {
        menu_gui_event = mLogf_common_relationalOP_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPERATOR);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_RELATIONAL_OP);
                symbol.set_valui(opt_dest);

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_ARITHMETICOP)
    {
        menu_gui_event = mLogf_common_arithmeticOP_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPERATOR);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_ARITHMETIC_OP);
                symbol.set_valui(opt_dest);

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_ASOCIATIVITYOP)
    {
        menu_gui_event = mLogf_common_asociativityOP_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPERATOR);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_ASSOCIATIVITY_OP);
                symbol.set_valui(opt_dest);

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_PUNCTUATIONMARKOP)
    {
        menu_gui_event = mLogf_common_punctuationmarkOP_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPERATOR);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_PUNCTUATION_MARKS_OP);
                symbol.set_valui(opt_dest);

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_ASSIGNAMENTOP)
    {
        menu_gui_event = mLogf_common_assignmentOP_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPERATOR);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_ASSIGNMENT_OP);
                symbol.set_valui(opt_dest);

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_LITERAL)
    {
        menu_gui_event = mLogf_common_literal_job(&valf);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                symbol.set_type(LOGF_SYMB_TYPE_OPRND_FLOAT);
                symbol.set_subtype(LOGF_SYMB_SUBTYPE_LITERALS);
                symbol.set_valf(valf);

                gpmenu[0].sm0 = MLOGF_APPLY_SYMBOL_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_VOLTAGE)
    {
        gpmenu[0].sm0 = MLOGF_ESCAPE;
    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_CURRENT)
    {
        gpmenu[0].sm0 = MLOGF_ESCAPE;

    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_TEMPERATURE)
    {
        gpmenu[0].sm0 = MLOGF_ESCAPE;

    }
    else if (gpmenu[0].sm0 == MLOGF_COMMON_FLAGS)
    {
        gpmenu[0].sm0 = MLOGF_ESCAPE;
    }

    if (gpmenu[0].sm0 == MLOGF_APPLY_SYMBOL_METHOD)//exit
    {
        switch (piz_action_subtype)
        {
            case PIZ_SYMBOL_ASSIGN_ACTION_NEW:
                Symbol.add(symbol);
                break;
            case PIZ_SYMBOL_ASSIGN_ACTION_REPLACE:
                Symbol.replace(symbol);
                break;
            case PIZ_SYMBOL_ASSIGN_ACTION_INSERT:
                Symbol.insert(symbol);
                break;
            case PIZ_SYMBOL_ASSIGN_ACTION_DELETE:
                break;
            case PIZ_SYMBOL_ASSIGN_ACTION_DELETEALL:
                break;
            default:
                break;
        }
        piz.action.subtype = piz_action_subtype;

        gpmenu[0].sm0 = MLOGF_ESCAPE;
    }

    if (gpmenu[0].sm0 == MLOGF_ENTER_ASSIGNMENT)
    {
        menu_gui_event = mLogf_assign_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == 0)
                    gpmenu[0].sm0 = MLOGF_ENTER_ASSIGN_RELAY;
                else
                    {
                        gpmenu[0].sm0 = MLOGF_ENTER_ASSIGN_FX;

                        vscroll.base_offset_reset();
                    }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    if (gpmenu[0].sm0 == MLOGF_ENTER_ASSIGN_RELAY)
    {
        menu_gui_event = mLogf_common_assign_relays_available_job(&opt_dest, piz_action_subtype);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                uint8_t rele_available[NUM_RELE_MAX];
                logf_outAssign.get_array_rele_available(rele_available);
                //
                logf_output.set_type(LOGF_ASSIG_TYPE_DIRECT_RELAY);
                uint8_t rele = rele_available[opt_dest>>1];
                logf_output.set_rele(rele);// div 2
                logf_output.set_rele_activeif(!(opt_dest%2));//modulo devuelve if active in
                //
                logf_rele[rele].bf.busy = 1;

                gpmenu[0].sm0 = MLOGF_APPLY_ASSIGN_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    else if (gpmenu[0].sm0 == MLOGF_ENTER_ASSIGN_FX)
    {
        menu_gui_event = mLogf_assigFx_functs_job(&opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                switch (opt_dest)
                {
                    case 0:
                        gpmenu[0].sm0 = MLOGF_ENTER_ASSIGN_FX_TIMER;

                        //vscroll.stack_push();//dangerous saving in this moment
                        break;

                    default:
                        break;
                }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }

    if (gpmenu[0].sm0 == MLOGF_ENTER_ASSIGN_FX_TIMER)
    {
        float valf;
        menu_gui_event = mLogf_common_assign_fx_timer_job((uint8_t*)&opt_dest, &valf, piz_action_subtype);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                uint8_t rele_available[NUM_RELE_MAX];
                logf_outAssign.get_array_rele_available(rele_available);

                uint8_t rele = rele_available[opt_dest>>1];
                logf_output.set_rele(rele);// div 2
                logf_output.set_rele_activeif(!(opt_dest%2));//modulo devuelve if active in
                //
                logf_output.set_valf(valf);//guarda el literal introducido
                logf_output.set_type(LOGF_ASSIG_TYPE_FX);
                logf_output.set_subtype(LOGF_ASSIG_SUBTYPE_FX_TIMER);
                //
                logf_rele[rele].bf.busy = 1;

                gpmenu[0].sm0 = MLOGF_APPLY_ASSIGN_METHOD;
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                gpmenu[0].sm0 = MLOGF_ENTER_ASSIGN_FX;//MLOGF_ESCAPE;

                //vscroll.stack_pop();
            }
        }
    }

    if (gpmenu[0].sm0 == MLOGF_APPLY_ASSIGN_METHOD)
    {
        switch (piz_action_subtype)
        {
            case PIZ_SYMBOL_ASSIGN_ACTION_NEW:
                logf_outAssign.add(logf_output);

                break;
            case PIZ_SYMBOL_ASSIGN_ACTION_REPLACE:
                logf_outAssign.replace(logf_output);
                break;
            case PIZ_SYMBOL_ASSIGN_ACTION_INSERT:
                logf_outAssign.insert(logf_output);
                break;
            case PIZ_SYMBOL_ASSIGN_ACTION_DELETE:
                break;
            case PIZ_SYMBOL_ASSIGN_ACTION_DELETEALL:
                break;
            default:
                break;
        }
        piz.action.subtype = piz_action_subtype;

        gpmenu[0].sm0 = MLOGF_ESCAPE;
    }

    //------------------------------------------------------------------
    if (gpmenu[0].sm0 == MLOGF_QUESTION_EXIT)
    {
        menu_gui_event = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Exit?"), &opt_dest);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == GENERIC_YESNO_SELECT_YES)
                {
                    gpmenu[0].sm0 = MLOGF_QUESTION_SAVE;
                }
                else//GENERIC_YESNO_SELECT_NO
                {

                    gpmenu[0].sm0 = MLOGF_ESCAPE;
                }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {

               gpmenu[0].sm0 = MLOGF_ESCAPE;
            }
        }
    }
    if (gpmenu[0].sm0 == MLOGF_QUESTION_SAVE)
    {
        int8_t menu_gui_event = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Save changes?"), &opt_dest);

        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == GENERIC_YESNO_SELECT_YES)
                {
                    cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
                    gpmenu[0].sm0 = MLOGF_EXIT_FROM_THIS_MENU;
                }
                else//GENERIC_YESNO_SELECT_NO
                {
                    cod_ret = MENU_GUI_EVENT_ESCAPE;
                    gpmenu[0].sm0 = MLOGF_EXIT_FROM_THIS_MENU;
                }
            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {

                gpmenu[0].sm0 = MLOGF_QUESTION_EXIT;
            }
        }
    }
    //------------------------------------------------------------------

    if (gpmenu[0].sm0 == MLOGF_ESCAPE)
    {
        gpmenu[0].sm0 = MLOGF_INIT;
    }

    if (gpmenu[0].sm0 == MLOGF_EXIT_FROM_THIS_MENU)
    {
        lcd.noBlink();
        gpmenu[0].sm0 = MLOGF_INIT;//leave with 0x00 for the next enter


    }
    return cod_ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_edit_opt_fxKey0(void)
{
}
static inline void mLogf_edit_opt_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_edit_opt_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_edit_opt_fxKey3(void)
{
    uint8_t opt_dest = vscroll.get_markPosc();
    if (opt_dest == 0)
        gpmenu[1].sm0 = 2;
    else if (opt_dest == 1)
        gpmenu[1].sm0 = 3;
    else if (opt_dest == 2)
        gpmenu[1].sm0 = 4;
    else if (opt_dest == 3)
        gpmenu[1].sm0 = 5;
}
static inline void mLogf_edit_opt_fxKey4(void)
{
    gpmenu[1].sm0 = 7;
}

PTRFX_retVOID const mLogf_edit_opt_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_edit_opt_fxKey0, mLogf_edit_opt_fxKey1, mLogf_edit_opt_fxKey2,
    mLogf_edit_opt_fxKey3, mLogf_edit_opt_fxKey4
};

int8_t mLogf_edit_opt(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;
    if (gpmenu[1].sm0 == 0)
    {
        gpmenu[1].sm1 = 0;
        gpmenu[1].sm0++;
    }
    if (gpmenu[1].sm0 == 1)
    {
        char db_logf_new[4][LCDAN_STR_MAX_SIZE];
        lcdan_str_lineformat_align_P(&db_logf_new[0][0],  PSTR("1] Replace"), 0);
        lcdan_str_lineformat_align_P(&db_logf_new[1][0],  PSTR("2] Insert"), 0);
        lcdan_str_lineformat_align_P(&db_logf_new[2][0],  PSTR("3] Delete"), 0);
        lcdan_str_lineformat_align_P(&db_logf_new[3][0],  PSTR("4] Delete all"), 0);
        if (gpmenu[1].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_edit_opt_fxKey);
            vscroll.init(db_logf_new, 4, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0, 0);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[Choose option]"));
            gpmenu[1].sm1 = -1;
        }
        vscroll.set_db(db_logf_new);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_3);
    }
    else if (gpmenu[1].sm0 == 2)
    {
        *opt_dest = PIZ_SYMBOL_ASSIGN_ACTION_REPLACE;
        gpmenu[1].sm0 = 6;

    }
    else if (gpmenu[1].sm0 == 3)
    {
        *opt_dest = PIZ_SYMBOL_ASSIGN_ACTION_INSERT;
        gpmenu[1].sm0 = 6;
    }
    else if (gpmenu[1].sm0 == 4)
    {
        *opt_dest = PIZ_SYMBOL_ASSIGN_ACTION_DELETE;
        gpmenu[1].sm0 = 6;

    }
    else if (gpmenu[1].sm0 == 5)
    {
        *opt_dest = PIZ_SYMBOL_ASSIGN_ACTION_DELETEALL;
        gpmenu[1].sm0 = 6;
    }

    if (gpmenu[1].sm0 == 6)
    {
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        gpmenu[1].sm0 = 0x00;
    }
    if (gpmenu[1].sm0 == 7)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        gpmenu[1].sm0 = 0x00;
    }
    return cod_ret;
}


///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_select_loginput_assign_fxKey0(void)
{
}
static inline void mLogf_select_loginput_assign_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_select_loginput_assign_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_select_loginput_assign_fxKey3(void)
{
    int8_t opt_dest = vscroll.get_markPosc();
    if (opt_dest == 0)
        gpmenu[1].sm0 = 2;
    else
        gpmenu[1].sm0 = 3;
}
static inline void mLogf_select_loginput_assign_fxKey4(void)
{
    gpmenu[1].sm0 = 4;
}
PTRFX_retVOID const mLogf_select_loginput_assign_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_select_loginput_assign_fxKey0, mLogf_select_loginput_assign_fxKey1, mLogf_select_loginput_assign_fxKey2,
    mLogf_select_loginput_assign_fxKey3, mLogf_select_loginput_assign_fxKey4
};
int8_t mLogf_select_loginput_assign(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;

    if (gpmenu[1].sm0 == 0)
    {
        gpmenu[1].sm1 = 0;
        gpmenu[1].sm0++;
    }
    if (gpmenu[1].sm0 == 1)
    {
        char db_loginput_assign[2][LCDAN_STR_MAX_SIZE];
        lcdan_str_lineformat_align_P(&db_loginput_assign[0][0],  PSTR("1] Logical Exp."), 0);
        lcdan_str_lineformat_align_P(&db_loginput_assign[1][0],  PSTR("2] Assignment"), 0);

        if (gpmenu[1].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_select_loginput_assign_fxKey);
            vscroll.init(db_loginput_assign, 2, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("[SELECT]"));
            gpmenu[1].sm1 = -1;
        }
        vscroll.set_db(db_loginput_assign);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_2);
    }
    else if (gpmenu[1].sm0 > 1)
    {
        if (gpmenu[1].sm0 == 2)
        {
            *opt_dest = 0;
            cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        }
        else if (gpmenu[1].sm0 == 3)
        {
            *opt_dest = 1;
            cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        }
        else if (gpmenu[1].sm0 == 4)
        {
            cod_ret = MENU_GUI_EVENT_ESCAPE;
        }
        gpmenu[1].sm0 = 0;
    }
    return cod_ret;
}

///////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_logexp_enter_symbol_fxKey0(void)
{
    //present all options
}
static inline void mLogf_logexp_enter_symbol_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_logexp_enter_symbol_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_logexp_enter_symbol_fxKey3(void)
{
    gpmenu[1].opt_sel = vscroll.get_markPosc();
    gpmenu[1].sm0 = 2;
}

static inline void mLogf_logexp_enter_symbol_fxKey4(void)
{
    gpmenu[1].sm0 = 3;
}
PTRFX_retVOID const mLogf_logexp_enter_symbol_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_logexp_enter_symbol_fxKey0, mLogf_logexp_enter_symbol_fxKey1, mLogf_logexp_enter_symbol_fxKey2,
    mLogf_logexp_enter_symbol_fxKey3, mLogf_logexp_enter_symbol_fxKey4
};

int8_t mLogf_logexp_enter_symbol_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;

    if (gpmenu[1].sm0 == 0)
    {
        gpmenu[1].sm0++;
        gpmenu[1].sm1=0x00;
    }
    if (gpmenu[1].sm0 == 1)
    {
#define DB_LOGF_EXP_NEW_SYMBOL_NUMMAX 10
        char db_logf_exp_new_symbol[DB_LOGF_EXP_NEW_SYMBOL_NUMMAX][LCDAN_STR_MAX_SIZE];

        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[0][0],    PSTR("1] Digital inputs"), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[1][0],    PSTR("2] Analog inputs"), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[2][0],    PSTR("3] Relays"), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[3][0],    PSTR("4] Logical OP."), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[4][0],    PSTR("5] Relational OP."), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[5][0],    PSTR("6] Arithmetic OP."), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[6][0],    PSTR("7] Associative OP."), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[7][0],    PSTR("8] Punctuation mark"), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[8][0],    PSTR("9] Assignment OP."), 0);
        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[9][0],    PSTR("10] Literals"), 0);

//        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[10][0],   PSTR("11] Voltage"), 0);
//        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[11][0],   PSTR("12] Current"), 0);
//        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[12][0],   PSTR("13] Temperature"), 0);
//        lcdan_str_lineformat_align_P(&db_logf_exp_new_symbol[13][0],   PSTR("14] Flags"), 0);

        if (gpmenu[1].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_logexp_enter_symbol_fxKey);
            vscroll.init(db_logf_exp_new_symbol, DB_LOGF_EXP_NEW_SYMBOL_NUMMAX, vscroll_dispShowBuff, 3, FEAT_MARK_ON, 0,0);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[VARIABLES/OPERATOR]"));
            gpmenu[1].sm1 = -1;
        }
        vscroll.set_db(db_logf_exp_new_symbol);
        vscroll.job();
        vscroll_lcd_show_disp();
    }
    else if (gpmenu[1].sm0 > 1)
    {
        if (gpmenu[1].sm0 == 2)
        {
            *opt_dest = gpmenu[1].opt_sel;
            cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        }
        else if (gpmenu[1].sm0 == 3)
        {
            cod_ret = MENU_GUI_EVENT_ESCAPE;
        }
        gpmenu[1].sm0 = 0;
    }
    return cod_ret;

}

////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_assign_fxKey0(void)
{
}
static inline void mLogf_assign_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_assign_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_assign_fxKey3(void)
{
    gpmenu[1].opt_sel = vscroll.get_markPosc();
    gpmenu[1].sm0 = 2;
}
static inline void mLogf_assign_fxKey4(void)
{
    gpmenu[1].sm0 = 3;
}
PTRFX_retVOID const mLogf_assign_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_assign_fxKey0, mLogf_assign_fxKey1, mLogf_assign_fxKey2, mLogf_assign_fxKey3, mLogf_assign_fxKey4
};
int8_t mLogf_assign_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;

    if (gpmenu[1].sm0 == 0)
    {
        gpmenu[1].sm1 = 0x00;
        gpmenu[1].sm0++;
    }
    if (gpmenu[1].sm0 == 1)
    {
        char db_logf_assignFx[2][LCDAN_STR_MAX_SIZE];
        lcdan_str_lineformat_align_P(&db_logf_assignFx[0][0],  PSTR("1] Relay Output"), 0);
        lcdan_str_lineformat_align_P(&db_logf_assignFx[1][0],  PSTR("2] Functions"), 0);

        if (gpmenu[1].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_assign_fxKey);
            vscroll.init(db_logf_assignFx, 2, vscroll_dispShowBuff, 2, FEAT_MARK_ON, vscroll.base, vscroll.offset);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[ASSIGNMENT OUTPUTS]"));
            gpmenu[1].sm1 = -1;
        }
        vscroll.set_db(db_logf_assignFx);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_2);
    }
    else if (gpmenu[1].sm0 == 2)
    {
        *opt_dest = gpmenu[1].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        gpmenu[1].sm0 = 0x00;
    }
    else if (gpmenu[1].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        gpmenu[1].sm0 = 0x00;
    }
    return cod_ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mLogf_assigFx_functs_fxKey0(void)
{
}
static inline void mLogf_assigFx_functs_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mLogf_assigFx_functs_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mLogf_assigFx_functs_fxKey3(void)
{
    gpmenu[1].opt_sel = vscroll.get_markPosc();
    gpmenu[1].sm0 = 2;
}
static inline void mLogf_assigFx_functs_fxKey4(void)
{
    gpmenu[1].sm0 = 3;
}
PTRFX_retVOID const mLogf_assigFx_functs_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mLogf_assigFx_functs_fxKey0, mLogf_assigFx_functs_fxKey1, mLogf_assigFx_functs_fxKey2,
    mLogf_assigFx_functs_fxKey3, mLogf_assigFx_functs_fxKey4
};

int8_t mLogf_assigFx_functs_job(int8_t *opt_dest)
{
    int8_t cod_ret = MENU_GUI_EVENT_WAITING_ACTION;

#define MLOGF_ASSIGNFX_FX_NUMMAX 1
    char db_logf_assignfx_functs[MLOGF_ASSIGNFX_FX_NUMMAX][LCDAN_STR_MAX_SIZE];

    lcdan_str_lineformat_align_P(&db_logf_assignfx_functs[0][0], PSTR("1] Timer(Rx,millis)"), 0);
    if (gpmenu[1].sm0 == 0)
    {
        gpmenu[1].sm1 = 0x00;
        gpmenu[1].sm0++;
    }
    //
    if (gpmenu[1].sm0 == 1)
    {
        if (gpmenu[1].sm1 == 0)
        {
            kb_change_keyDo_pgm(mLogf_assigFx_functs_fxKey);
            vscroll.init(db_logf_assignfx_functs, MLOGF_ASSIGNFX_FX_NUMMAX, vscroll_dispShowBuff, MLOGF_ASSIGNFX_FX_NUMMAX, FEAT_MARK_ON, 0,0);// vscroll.base, vscroll.offset);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[SELECT FUNCTION]"));
            gpmenu[1].sm0 = -1;
        }
        vscroll.set_db(db_logf_assignfx_functs);
        vscroll.job();
        vscroll_lcd_show_disp(LCD_ROW_1, LCD_ROW_1);
    }
    else if (gpmenu[1].sm0 == 2)
    {
        *opt_dest = gpmenu[1].opt_sel;
        cod_ret = MENU_GUI_EVENT_OPTION_SELECTED;
        gpmenu[1].sm0 = 0x00;
    }
    else if (gpmenu[1].sm0 == 3)
    {
        cod_ret = MENU_GUI_EVENT_ESCAPE;
        gpmenu[1].sm0 = 0x00;
    }
    //
    return cod_ret;
}
