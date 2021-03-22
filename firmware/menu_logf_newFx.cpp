/****************************************************************************
* Project: Battery System
*
* 2016-09-12-23.20
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
#include "menu.h"
#include "lcd_fxaux.h"
#include "vscroll.h"
//#include "sysprint.h"
//#include "logf_class.h"
#include "logf.h"
//#include "logf_menu_common.h"
//#include "setnum.h"
//#include "piz_class.h"
//#include "piz.h"
#include <EEPROM.h>
#include "myEEPROM.h"
#include "menu_logf_newFx.h"
#include "mLogf_logeditor.h"

void mLogf_newFx_job(void)
{
    int8_t menu_gui_event;
    //    int8_t opt_dest;
    //    char buff[4];
    //    char str[LCDAN_STR_MAX_SIZE];

    if (thismenu_sm0 == 0)
    {
        menu_gui_event = mLogf_logeditor_job(PSTR("New log.Exp"), num_exp_saved+1);
        if (menu_gui_event != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (menu_gui_event == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                //++--
                //save in eeprom
                num_exp_saved++;//numero de expresiones guardadas
                if (num_exp_saved > LOGEXP_EEPROM_NUM_EXP_SAVED)
                {
                    num_exp_saved = 1;//chancara a sgte.
                }
                EEPROM.put(EEPROM_ADDR_LOGF_EXP_NUM_EXP_SAVED, num_exp_saved);

                EEPROM.put(EEPROM_ADDR_LOGF_LOGEXP_ASSIGOUT_DATA + (sizeof(struct _logf_logExp_assigOut_data) )*(num_exp_saved-1),
                           logf_logExp_assigOut_data );

                for (uint8_t i=0; i< NUM_RELE_MAX; i++)
                {
                    EEPROM.put(EEPROM_ADDR_RELE_BUSY + ( sizeof(struct _logf_rele_bf) * i ) , logf_rele[i].bf );
                }

            }
            else if (menu_gui_event == MENU_GUI_EVENT_ESCAPE)
            {
                for (uint8_t i=0; i<NUM_RELE_MAX; i++)//reload. dejar intacto los flags de rele busy
                {
                    EEPROM.get(EEPROM_ADDR_RELE_BUSY + ( sizeof(struct _logf_rele_bf) * i ) , logf_rele[i].bf );
                }
            }
            vscroll.stack_pop();
            menu_setSubMenu(MGO_MLOGF_MAIN_JOB);
        }
    }

}
