#include <Arduino.h>
#include "timedate.h"
#include "ikey.h"
#include "system.h"
#include "kb.h"


#include "menu_mainMenu.h"
#include "menu_bkground.h"//only for #defines TASKMAN_SIGN_ENTER etc


static void set_timedate_print_hh(uint8_t hh);
static void set_timedate_print_mm(uint8_t mm);
static void setTimeDate_print(uint8_t set_now);

struct _setTimeDate
{
    int8_t hh;
    int8_t mm;
    int8_t ss;
    int8_t ampm;

    uint8_t set_hms;

    //uint8_t sm0;//x scheduler

    //
    uint8_t sm1;//x blink
    unsigned long last_millis;
    //
    struct _bf
    {
        unsigned print_time:1;
        unsigned blink:1;
        unsigned __a:6;
    } bf;

} setTimeDate;

static void set_time_fxKey0(void)//menu
{
}
static void set_time_fxKey1(void)//up
{
    if (setTimeDate.set_hms == 0)
    {
        if (++setTimeDate.hh > 12)
        {
            setTimeDate.hh = 1;
        }
    }

    if (setTimeDate.set_hms == 1)
    {
        if (++setTimeDate.mm > 59)
        {
            setTimeDate.mm = 0;
        }
    }

    if (setTimeDate.set_hms == 2)
    {
        if (++setTimeDate.ampm>1)
            setTimeDate.ampm = 0;

    }




    setTimeDate.bf.print_time = 1;
    setTimeDate.bf.blink = 0x00;
    setTimeDate.last_millis = millis();
    setTimeDate.sm1 = 1;

}
static void set_time_fxKey2(void)//down
{
    if (setTimeDate.set_hms == 0)
    {
        if (--setTimeDate.hh < 1)
        {
            setTimeDate.hh = 12;
        }
    }

    if (setTimeDate.set_hms == 1)
    {
        if (--setTimeDate.mm < 0)
        {
            setTimeDate.mm = 59;
        }
    }

    if (setTimeDate.set_hms == 2)
    {
        if (--setTimeDate.ampm < 0)
            setTimeDate.ampm = 1;

    }


    setTimeDate.bf.print_time = 1;
    setTimeDate.bf.blink = 0x00;//fijar el param. por 0.5s
    //
    setTimeDate.sm1 = 1;        //empezar nueva temp. despues de ello
    setTimeDate.last_millis = millis();

}
static void set_time_fxKey3(void)//enter
{
    setTimeDate_print(setTimeDate.set_hms);//print the current param

    if (++setTimeDate.set_hms > 2)
        setTimeDate.set_hms = 0x00;

    //
    setTimeDate.sm1 = 2;
    setTimeDate.bf.blink = 1;

}
static void set_time_fxKey4(void)//scape
{
   kb_stackPop();

    taskman_mainMenu.sm0 = 0;

}

PTRFX_retVOID const set_time_fxKey[KB_NUM_KEYS] PROGMEM =
{set_time_fxKey0, set_time_fxKey1, set_time_fxKey2, set_time_fxKey3, set_time_fxKey4};



struct _taskman_setTime taskman_setTime;

void set_timedate(void);

void mainMenu_setTime_job(void)
{
    if (taskman_setTime.sign == TASKMAN_SIGN_ENTER)
    {
        //taskman_setDate.reInitJob = 1;
        taskman_setTime.sm0 = 0x00;

        taskman_setTime.sign = TASKMAN_SIGN_CLEAR;
    }

    set_timedate();

}

void set_timedate(void)
{
    uint8_t col;

    if (taskman_setTime.sm0 == 0)
    {
        kb_stackPush();
        kb_change_keyDo_pgm(set_time_fxKey);

        lcd.clear();
        sysPrint_lcd_serialPrintln(0,0,"[SET TIME]");
        sysPrint_lcd_serialPrintln(1,6,"(hh:mm)");

        setTimeDate.hh = 1;

        set_timedate_print_hh(setTimeDate.hh);
        lcd.printAtPosc(2,9,":");//blank space
        set_timedate_print_mm(setTimeDate.mm);

        sysPrint_lcd_serialPrintln(3,8,"AM");


        setTimeDate.sm1 = 1;
        setTimeDate.bf.blink = 0;
        setTimeDate.last_millis = millis();
        //

        setTimeDate.bf.print_time = 1;

        taskman_setTime.sm0 = -1;
    }

    //_____________________________________________________
    if (setTimeDate.bf.print_time)//refresh LCD
    {
        setTimeDate.bf.print_time = 0;

        setTimeDate_print(setTimeDate.set_hms);
    }

    //_____________________________________________________
    //blink position
//    if (setTimeDate.sm1 == 0)
//    {
//        setTimeDate.last_millis = millis();
//        setTimeDate.sm1++;
//    }

    if (setTimeDate.sm1 == 1)
    {
        if ( (millis() - setTimeDate.last_millis) > 500)
        {
            setTimeDate.bf.blink = !setTimeDate.bf.blink;
            setTimeDate.sm1++;
        }
    }

    if (setTimeDate.sm1 == 2)
    {
        if (setTimeDate.bf.blink)
        {
            if (setTimeDate.set_hms == 0)
                lcd.printAtPosc(2,7,"  ");//blank space
            else if (setTimeDate.set_hms == 1)
                lcd.printAtPosc(2,10,"  ");//blank space
            else if (setTimeDate.set_hms == 2)
                lcd.printAtPosc(3,8,"  ");//blank space
        }
        else
        {
            setTimeDate_print(setTimeDate.set_hms);
        }
        //
        setTimeDate.last_millis = millis();//DIRECT
        //
        setTimeDate.sm1 = 1;//0x00;

    }

}


static void set_timedate_print_hh(uint8_t hh)
{

    uint8_t col= 7;
    if (hh < 10)
        lcd.printAtPosc(2,col++,"0");//blank space

    lcd.printAtPosc(2,col,String(hh));
}

static void set_timedate_print_mm(uint8_t mm)
{
    uint8_t col = 10;

    if (mm < 10)
        lcd.printAtPosc(2,col++,"0");//blank space

    lcd.printAtPosc(2,col,String(mm));
}

static void set_timedate_print_ampm(uint8_t ampm )
{
    if (ampm == 0)
    {
        lcd.printAtPosc(3,8,"AM");
    }
    else
    {
        lcd.printAtPosc(3,8,"PM");
    }

}

static void setTimeDate_print(uint8_t set_now)
{
    if (set_now == 0)
        set_timedate_print_hh(setTimeDate.hh);
    else if (set_now == 1)
        set_timedate_print_mm(setTimeDate.mm);
    else if (set_now == 2)
        set_timedate_print_ampm(setTimeDate.ampm);

}
