//menu contact
#include <Arduino.h>
#include "system.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"
#include "sdcard.h"
#include "menu.h"
#include "menu_mainMenu.h"
#include "menu_contact.h"

static inline void mContact_menu_fxKey0(void)
{
}
static inline void mContact_menu_fxKey1(void)
{
    thismenu_sm1 = 0x00;

    if (--thismenu_sm0 < 2)
        thismenu_sm0 = 4;
}
static inline void mContact_menu_fxKey2(void)
{
    thismenu_sm1 = 0x00;

    if (++thismenu_sm0 > 4)
        thismenu_sm0 = 2;
}
static inline void mContact_menu_fxKey3(void)
{
    thismenu_sm1 = 0x00;

    if (++thismenu_sm0 > 4)
        thismenu_sm0 = 2;
}
static inline void mContact_menu_fxKey4(void)
{
    thismenu_sm0_last = thismenu_sm0;
    thismenu_sm0 = 5;
}

PTRFX_retVOID const mContact_menu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mContact_menu_fxKey0, mContact_menu_fxKey1, mContact_menu_fxKey2,
    mContact_menu_fxKey3, mContact_menu_fxKey4
};


void mContact_menu_job(void)
{
    char setting_value[SDCARD_SETTING_VALUE_MAX_LENGTH];

    if (thismenu_sm0 == 0)
    {
        thismenu_sm0_last = 2;
        thismenu_sm0++;
    }
    if (thismenu_sm0 == 1)
    {
        kb_change_keyDo_pgm(mContact_menu_fxKey);
        lcd.clear();
        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[CONTACT]"));
        thismenu_sm1 = 0;
        thismenu_sm0 = thismenu_sm0_last;
    }

    if (thismenu_sm0 == 2 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("Technical Support:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/setup.txt"),PSTR("TechnicalSupport"),setting_value) )
                sysPrint_lcdPrint(LCD_ROW_3, STR_CENTER, String(setting_value) );
            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 == 3 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("Support Contact:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/setup.txt"),PSTR("SupportContact"),setting_value) )
                sysPrint_lcdPrint(LCD_ROW_3, STR_CENTER, String(setting_value) );

            thismenu_sm1 = -1;
        }
    }
    if (thismenu_sm0 == 4 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("Support Phone:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/setup.txt"),PSTR("SupportPhone"),setting_value) )
                sysPrint_lcdPrint(LCD_ROW_3, STR_CENTER, String(setting_value) );

            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 == 5)
    {
        uint8_t cod_ret = menu_generic_yesno(MGENERIC_CAPTION_EXIT);
        if (cod_ret != MENU_RESP_WAITING)
        {
            if (cod_ret == MENU_RESP_YES)
            {
                //->exit
                vscroll.stack_pop();
                menu_setMenu(MGO_MAIN_MENU);
            }
            else if (cod_ret == MENU_RESP_NO)
                thismenu_sm0 = 1;//
        }
    }
}
