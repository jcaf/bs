/****************************************************************************
* Project: Battery System
*
* 2016-10-11-21.31
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
#include "vscroll.h"
#include "logf_class.h"
#include "logf.h"
#include "piz_class.h"

class _piz piz;

char db_logf_exp_new[DB_LOGF_LOGEXP_NUM_LINES_INBOARD][LCDAN_STR_MAX_SIZE];//DB_LOGF_LOGEXP_NUM_LINES_INBOARD defined in logf_class.h
VScroll vscrollpiz;//podria llamar al constructor


void piz_init(void)
{
//    piz.viewport.numline = 1;
    vscrollpiz.init(db_logf_exp_new, 1,
                    vscroll_dispShowBuff,
                    1,
                    FEAT_MARK_ON, 0, 0);
//
//    piz.action.type = PIZ_IDLE_ACTION;
    //set local variables

    piz.builder_content_set_dbstring(db_logf_exp_new);//builder content puede ser dividio en su propia clase. ok
    piz.vp.set_vscroll(&vscrollpiz);
    piz.prop.margen_right = 18;

}
void piz_man(void)
{
    piz.management();
}
