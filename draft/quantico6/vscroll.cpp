#include <Arduino.h>
#include "vscroll.h"
#include "lcd_fxaux.h"

void VScroll::init(String * db_data, VSCROLLDT db_dbsize, String * dispShowBuff_data, VSCROLLDT  dispShowBuff_numline,
                   uint8_t mark,//feature
                   VSCROLLDT base,
                   VSCROLLDT offset
                  )
{
    this->set_db(db_data);//db.data = db_data;
    db.dbsize   = db_dbsize;

    dispShowBuff.data = dispShowBuff_data ;

    //dispShowBuff.numline =  dispShowBuff_numline;
    if (db_dbsize <= LCD_ROW)//4
    {
        if (dispShowBuff_numline<=db_dbsize)
            dispShowBuff.numline = dispShowBuff_numline;
        else //error-> truncar
            dispShowBuff.numline = db_dbsize;
    }
    else
    {
        if (dispShowBuff_numline <= LCD_ROW)
            dispShowBuff.numline = dispShowBuff_numline;
        else
            dispShowBuff.numline = LCD_ROW;
    }

    db.base     = base;
    db.offset   = offset;
    feature.bf.mark = mark;//on off mark "<"
    dispShowBuff_filldata();

    //if (feature.bf.mark)
    //    add_mark( &dispShowBuff.data[db.offset] );//at Offset actual
}
void VScroll::set_db(String * db_data)//Util for local-variable
{
    db.data = db_data;
}

void VScroll::feat_mark_on(void)
{
    feature.bf.mark = 1;//on off mark "<"
}

void VScroll::feat_mark_off(void)
{
    feature.bf.mark = 0;//on off mark "<"
}

void VScroll::add_mark(String * pstr)
{
    char arr[LCD_COL + 1];
    pstr->toCharArray(arr, LCD_COL + 1);
    arr[LCD_COL - 1] = '<';
    *pstr = arr;
}

void VScroll::del_mark(String * pstr)
{
    char arr[LCD_COL + 1];
    pstr->toCharArray(arr, LCD_COL + 1);
    arr[LCD_COL - 1] = ' ';
    *pstr = arr;
}

void VScroll::dispShowBuff_filldata(void)
{
    uint8_t i;
    VSCROLLDT db_base = db.base;

    for (i = 0; i < dispShowBuff.numline; i++)
    {
        dispShowBuff.data[i] = db.data[db_base];

        if (++db_base == db.dbsize)
            db_base = 0x00;
    }

    if (feature.bf.mark)
        add_mark( &dispShowBuff.data[db.offset] );
}
/*
//this algorithm works! but, the cpu has to doing more calc.
void VScroll::process_signal(void)
{
    VSCROLLDT dboffset_right;
    VSCROLLDT dboffset_limit;
    VSCROLLDT dbbase_right;
    VSCROLLDT dbbase_limit;
    uint8_t _xor;
    int8_t k,kk;

    //1.- set values
    if (signal == SIGN_UP)
    {
        k= -1;
        dboffset_right = 0;

        if (db.dbsize <= dispShowBuff.numline)
            dboffset_limit = dispShowBuff.numline-1;
        else
            dboffset_limit = dboffset_right;

        dbbase_right = 0;
        dbbase_limit = db.dbsize -1;
        _xor = 1;
        //if ( (base > 0) ^ 1)
        //base = set_sup;
        kk = +1;
    }
    else
    {
        k = +1;
        dboffset_right = dispShowBuff.numline;

        if (db.dbsize <= dispShowBuff.numline)
            dboffset_limit = 0;
        else
            dboffset_limit = dboffset_right -1;

        dbbase_right = db.dbsize;
        dbbase_limit = 0;
        _xor = 0;
        //if ( (base > set_sup) ^ 0 )
        //base = set_inf
        kk = -1;
    }

    //2.- do algorithm
    if (db.dbsize <= dispShowBuff.numline)
    {
        if (feature.bf.mark)
            del_mark(&dispShowBuff.data[db.offset]);

        db.offset += k;
        if ( (db.offset >= dboffset_right ? 1:0) ^ _xor )
            db.offset = dboffset_limit;
    }
    else
    {
        db.offset += k;

        if ( ( db.offset >= dboffset_right ? 1:0) ^ _xor )
        {
            db.offset = dboffset_limit;

            db.base += k;
            if ((db.base >= dbbase_right? 1:0) ^ _xor)
                db.base = dbbase_limit;
        }
        else
        {
            if (feature.bf.mark)
                del_mark(&dispShowBuff.data[db.offset + kk]);//+/- 1
        }

    }
}
*/

void VScroll::process_signal(void)
{
    VSCROLLDT limit_sup = dispShowBuff.numline - 1;

    if (signal == SIGN_UP)
    {
        if (db.dbsize <= dispShowBuff.numline)
        {
            if (feature.bf.mark)
                del_mark(&dispShowBuff.data[db.offset]);

            if (--db.offset < 0 )
                db.offset = limit_sup;
        }
        else
        {
            db.offset--;
            if ( db.offset < 0)
            {
                db.offset = 0;

                if (--db.base < 0)
                    db.base = db.dbsize-1;
            }
            else
            {
                if (feature.bf.mark)
                    del_mark(&dispShowBuff.data[db.offset + 1]);
            }
        }
    }
    else
    {
        if (db.dbsize <= dispShowBuff.numline)
        {
            if (feature.bf.mark)
                del_mark(&dispShowBuff.data[db.offset]);

            if (++db.offset > limit_sup)
                db.offset = 0;
        }
        else
        {
            db.offset++;
            if ( db.offset > limit_sup )
            {
                db.offset = limit_sup ;

                if (++db.base >= db.dbsize)
                    db.base = 0x00;
            }
            else
            {
                if (feature.bf.mark)
                    del_mark(&dispShowBuff.data[db.offset - 1]);
            }
        }
    }
}


VSCROLLDT VScroll::get_markPosc(void)
{
    VSCROLLDT r =  db.base + db.offset ;
    if ( r >= db.dbsize )
        r = r - db.dbsize;
    return r;
    //return (uint16_t)(r);
}

void VScroll::stack_push(void)
{
    vscrollst.stack[vscrollst.sp].base = db.base;       //vscrollst.base;
    vscrollst.stack[vscrollst.sp].offset = db.offset;   //vscrollst.offset;

    if (++vscrollst.sp >= VSCROLL_STACK_DEEP)
        vscrollst.sp = 0;
}
void VScroll::stack_pop(uint8_t n)//1 pop x default
{
    for (uint8_t i=0; i<n; i++)
    {
        if (--vscrollst.sp < 0)
            vscrollst.sp = VSCROLL_STACK_DEEP;
    }

    base = vscrollst.stack[vscrollst.sp].base;
    offset = vscrollst.stack[vscrollst.sp].offset;
}
void VScroll::base_offset_reset(void)
{
    base = 0;
    offset = 0;
}

void VScroll::sign_up(void)
{
    signal = SIGN_UP;
}
void VScroll::sign_down(void)
{
    signal = SIGN_DOWN;
}
void VScroll::clear_sign(void)
{
    signal = SIGN_CLEAR;
}
void VScroll::job(void)
{
    if (signal != SIGN_CLEAR)
    {
        process_signal();
        clear_sign();
    }

    dispShowBuff_filldata();
}

//hay 2 tipos de jobs...statics y dynamics
VScroll vscroll;
String dispShowBuff[DISP_SHOW_BUFF_LENGTH];

