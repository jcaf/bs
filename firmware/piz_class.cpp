/****************************************************************************
* Project: Battery System
*
* 2016-10-14-12.32
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

void _piz_el::set_coord(PIZ_EL_COORD coord)
{
    this->coord = coord;
}
PIZ_EL_COORD _piz_el::get_coord(void)
{
    return coord;
}

void _piz_el::set_coord_f(int8_t f)
{
    coord.f = f;
}
void _piz_el::set_coord_c(int8_t c)
{
    coord.c = c;
}
int8_t _piz_el::get_coord_f(void)
{
    return coord.f;
}
int8_t _piz_el::get_coord_c(void)
{
    return coord.c;
}

void _piz_el::set_type(int8_t type)
{
    this->type = type;
}
void _piz_el::set_subtype(int8_t subtype)
{
    this->subtype = subtype;
}
int8_t _piz_el::get_type(void)
{
    return type;
}
int8_t _piz_el::get_subtype(void)
{
    return subtype;
}
//////////////////////////////////////////////////////////////////
void _piz_el_mgt::add(const class _piz_el &el)
{
    el_arr[ptr] = el;
    ptr++;//deja incrementado
}

int8_t _piz_el_mgt::getptr(void)
{
    return ptr;
}
void _piz_el_mgt::setptr(int8_t ptr)
{
    this->ptr = ptr;
}
void _piz_el_mgt::resetptr(void)
{
    ptr = 0x00;
}
void _piz_el_mgt::incptr(void)
{
    ptr++;
}
void _piz_el_mgt::decptr(void)
{
    ptr--;
}
uint8_t _piz_el_mgt::get_length(void)//include ::NEXT
{
    uint8_t length=0;
    while (el_arr[length].get_type() != _ELEMENT_TYPE::EOV)//next
    {
        length++;
    }
    return length;
}

int8_t _piz_el_mgt::get_type(void)
{
    return el_arr[ getptr() ].get_type();
}

int8_t _piz_el_mgt::get_subtype(void)
{
    return el_arr[ getptr() ].get_subtype();
}

PIZ_EL_COORD _piz_el_mgt::get_coord(void)
{
    return el_arr[ getptr() ].get_coord();
}

void _piz_el_mgt::save(int8_t ptr)
{
    el_saved.el = el_arr[ptr];
    el_saved.ptr = ptr;
}
class _el_saved _piz_el_mgt::get_saved(void)
{
    return el_saved;
}

class _piz_el _piz_el_mgt::get_pointed_element(void)
{
    return el_arr[this->ptr];//actual ptr
}
class _piz_el _piz_el_mgt::get_element(int8_t i)
{
    return el_arr[i];
}

//
//////////////////////////////////////////////////////////////////
void _piz_viewport::set_lastconfig(PIZ_VP_CONFIG coord)
{
    this->coord = coord;
}
PIZ_VP_CONFIG _piz_viewport::get_lastconfig(void)
{
    return coord;
}
void _piz_viewport::set_numline(int8_t nl)
{
    numline = nl;
}
int8_t _piz_viewport::get_numline(void)
{
    return numline;
}
void _piz_viewport::set_vscroll(VScroll *pvscroll)
{
    this->pvscroll = pvscroll;
}
//////////////////////////////////////////////////////////////////
void _piz_build_dbstring::str_reset(void)
{
    str[0] = '\0';
}
void _piz_build_dbstring::set_numline(int8_t nl)
{
    numline = nl;
}
int8_t _piz_build_dbstring::get_numline(void)
{
    return numline;
}

void _piz_build_dbstring::add_row(int8_t line, char *str)
{
    lcdan_str_lineformat_align(&pdbstring[line][0], str, 0);
}
void _piz_build_dbstring::build_line_content(void)
{
    strcat(str, text_out);
    if (bf.add_blankspace == 1)
        strcat(str, " ");

    add_row(get_numline(), str);
}
void _piz_build_dbstring::set_addBlankspace(uint8_t bs)
{
    bf.add_blankspace = bs;
}
//////////////////////////////////////////////
void _piz::builder_content_set_dbstring(char (*pdbstring)[LCDAN_STR_MAX_SIZE])
{
    this->pdbstring = pdbstring;
}


static int8_t f_last;

//void
struct _builder_state
_piz::builder_content(void)//set board's param
{
    struct _builder_state builder_state;

    uint8_t strlen_actual;
    class _piz_build_dbstring piz_build_dbstring(pdbstring);//add constructor pointer from "piz object"
    //
    prop.numlines = 0;
    prop.inthisline.counter_el=0;

    PIZ_EL_COORD coord= {0,0};

    uint8_t inthisline_counter_chars=0;
    piz_build_dbstring.str_reset();

    el.resetptr();

    class _piz_el piz_el;
    piz_el.set_type(_ELEMENT_TYPE::LOGF_SYMB);//all elements will be saved as _ELEMENT_TYPE::LOGF_SYMB
    //
    for (uint8_t e=0; e < Symbol.get_length(); e++)
    {
        Symbol.get_text(e, piz_build_dbstring.text_out);//registrar
        strlen_actual = strlen(piz_build_dbstring.text_out);
        inthisline_counter_chars += strlen_actual;

        piz_build_dbstring.set_addBlankspace(0);

        if (inthisline_counter_chars < prop.margen_right)
        {
            //
            piz_el.set_coord(coord);
            el.add(piz_el);
            //
            inthisline_counter_chars+= PIZ_BLANKSPACE_NUMCHARS;
            //
            piz_build_dbstring.set_addBlankspace(1);
            piz_build_dbstring.set_numline(coord.f);
            piz_build_dbstring.build_line_content();
            //
        }
        else if (inthisline_counter_chars == prop.margen_right)
        {
            //
            piz_el.set_coord(coord);
            el.add(piz_el);
            //
            prop.line[coord.f].num_el = prop.inthisline.counter_el;//save num elem

            piz_build_dbstring.set_addBlankspace(0);//no blankspace
            piz_build_dbstring.set_numline(coord.f);
            piz_build_dbstring.build_line_content();
            piz_build_dbstring.str_reset();

            inthisline_counter_chars = 0;
            coord.f++;//leave cursor to newline
            //
            piz_build_dbstring.add_row(coord.f, (char *)"");
        }
        else if (inthisline_counter_chars > prop.margen_right)
        {
            prop.line[coord.f].num_el = prop.inthisline.counter_el;//save num elem

            coord.f++;
            coord.c = 0;
            //
            piz_el.set_coord(coord);
            el.add(piz_el);
            //
            prop.inthisline.counter_el =  0;

            inthisline_counter_chars = strlen_actual + PIZ_BLANKSPACE_NUMCHARS;
            //
            piz_build_dbstring.str_reset();
            piz_build_dbstring.set_addBlankspace(1);
            piz_build_dbstring.set_numline(coord.f);
            piz_build_dbstring.build_line_content();
            //
        }
        coord.c = inthisline_counter_chars;//for the next
        prop.inthisline.counter_el++;
    }

    //...........Assigment............................................................
    char text_out[LCDAN_STR_MAX_SIZE];
    piz_el.set_type(_ELEMENT_TYPE::ASSIGNMENT);//all elements will be saved as _ELEMENT_TYPE::ASSIGNMENT
    for (uint8_t e=0; e< logf_outAssign.get_length(); e++)
    {
        text_out[0]='\0';

        logf_outAssign.get_text(e, text_out);
        //append new line
        if (e==0)
        {
            coord.f++;
        }

        coord.c = 2;//set fixed location after "->"T
        //
        piz_el.set_coord(coord);
        el.add(piz_el);
        //
        piz_build_dbstring.add_row(coord.f, text_out);
        coord.f++;
        piz_build_dbstring.add_row(coord.f, (char *)"->");
    }

    prop.numlines = coord.f +1;
f_last = coord.f;

    //
    builder_state.cursor.nextpos = coord.c;

    //artilugio add to the end of vector the element POINTER_TO_NEXT
    uint8_t ptr = el.getptr();
    el.el_arr[ptr].set_coord(coord);
    el.el_arr[ptr].set_type(_ELEMENT_TYPE::POINTER_TO_NEXT);
    //

    //added
    if (action.type != PIZ_IDLE_ACTION)
    {

        if ((action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_REPLACE) ||(action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_INSERT)
            ||(action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_NONE)
            )
        {
            class _el_saved el_saved = el.get_saved();

            if (action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_NONE)
            {
                el.setptr( el_saved.ptr);//for escape(none)
            }
            else
            {
                el.setptr( el_saved.ptr+1 );//equalize(match) after replace or insert
            }
        }
    }

    //
    return builder_state;
}

void _piz::viewport(void)
{
    VSCROLLDT base;
    VSCROLLDT offset;

    if (action.type == PIZ_SYMBOL_ACTION)
    {
        if (action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_NEW)
        {
            if (prop.numlines > 0)
            {
               if (prop.numlines > 3)
                {
                    vp.numline = 3;

                    base = (prop.numlines - 3);
                    offset = vp.numline-1;
                }
                else
                {
                    base = 0;
                    vp.numline = prop.numlines;
                    offset = vp.numline -1;
                }
                vp.pvscroll->set_dispShowBuff_numline(vp.numline);
                vp.pvscroll->set_db_dbsize(prop.numlines);
                vp.pvscroll->set_db_base(base);
                vp.pvscroll->set_db_offset(offset);
                //
                vp.pvscroll->job();
            }
            else
            {
                vp.pvscroll->job();
            }
        }
        else if ((action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_REPLACE)||(action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_INSERT)
                 ||(action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_NONE)//escaped was pressed
                 )
        {
            //---------------
            if ( prop.numlines > 3)
                vp.numline = 3;//#define VIEWPORT_NUM_LINES_MAX 3
            else
                vp.numline = prop.numlines;

            vp.pvscroll->set_dispShowBuff_numline(vp.numline);
            vp.pvscroll->set_db_dbsize(prop.numlines);
            //---------------

            PIZ_VP_CONFIG piz_vp_lastconfig = vp.get_lastconfig();

            int8_t vp_room = piz_vp_lastconfig.base + (vp.numline-1);
            //
            class _el_saved el_saved = el.get_saved();
            int8_t el_saved_coord_f = el_saved.el.get_coord_f();
            //
            int8_t el_next_f;// = el.el_arr[ el_saved.ptr+1 ].get_coord_f();//to next-pointer

            if (action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_NONE)
            {
                el_next_f = el.el_arr[ el_saved.ptr].get_coord_f();//to current-pointer
            }
            else
            {
                el_next_f = el.el_arr[ el_saved.ptr+1 ].get_coord_f();//to next-pointer
            }

            f_last = el_next_f;

            int8_t diff = 0;

            if (el_next_f > el_saved_coord_f)
            {
                diff = el_next_f - el_saved_coord_f;

                if (el_next_f > vp_room)
                {
                    base = piz_vp_lastconfig.base + diff;
                    offset = (vp.numline-1);
                }
                else
                {
                    base    = piz_vp_lastconfig.base;
                    offset  = piz_vp_lastconfig.offset + diff;
                }

                vp.pvscroll->set_db_base(base);
                vp.pvscroll->set_db_offset(offset);         //(< point to this line)
            }
            else
            {
                base    = piz_vp_lastconfig.base;
                offset  = piz_vp_lastconfig.offset;

                vp.pvscroll->set_db_base(base);
                vp.pvscroll->set_db_offset(offset);         //(< point to this line)
            }

            vp.pvscroll->job();
        }
    }
    else if (action.type == PIZ_CURSOR_ACTION)
    {
        if (action.subtype == PIZ_CURSOR_ACTION_RIGHT)
        {
            vp.pvscroll->sign_down();
        }
        else//PIZ_CURSOR_ACTION_LEFT
        {
            vp.pvscroll->sign_up();
        }
        vp.pvscroll->job();
    }

    vscroll_lcd_show_disp(LCD_ROW_1, (LCD_ROW_1+ vp.numline-1) );
}
//////////////////////////////////////////////////////////////////
void _piz::management(void)
{
    struct _builder_state builder_state;
    PIZ_EL_COORD coord;

    if (action.type == PIZ_SYMBOL_ACTION)
    {
        builder_state = builder_content();
        viewport();

        //
        if (action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_NEW)
        {
            cursor.set_c(builder_state.cursor.nextpos);
        }
        else if ((action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_REPLACE)||(action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_INSERT)
                || (action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_NONE)
                 )
        {
            class _el_saved el_saved = el.get_saved();

            if (action.subtype == PIZ_SYMBOL_ASSIGN_ACTION_NONE)
            {
                coord = el.el_arr[ el_saved.ptr ].get_coord();
            }
            else
            {
                coord = el.el_arr[ el_saved.ptr+1 ].get_coord();
            }

            cursor.set_c(coord.c);
        }
        //
    }
    else if (action.type == PIZ_CURSOR_ACTION)
    {
        coord = el.get_coord();
        if (coord.f != f_last)//if (fchange())
        {
            f_last = coord.f;
            viewport();
        }
        cursor.set_c(coord.c);
    }
    cursor.set_f(LCD_ROW_1+ vp.pvscroll->get_db_offset());
    cursor.blinkat_fc();
}
