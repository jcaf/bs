#include <Arduino.h>
#include "system.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"
#include "sdcard.h"
#include "menu.h"
#include "menu_mainMenu.h"
#include "menu_information.h"

static inline void mInformation_menu_fxKey0(void)
{
}
static inline void mInformation_menu_fxKey1(void)
{
    thismenu_sm1 = 0x00;

    if (--thismenu_sm0 < 2)
        thismenu_sm0 = 4;
}
static inline void mInformation_menu_fxKey2(void)
{
    thismenu_sm1 = 0x00;

    if (++thismenu_sm0 > 4)
        thismenu_sm0 = 2;
}
static inline void mInformation_menu_fxKey3(void)
{
    thismenu_sm1 = 0x00;

    if (++thismenu_sm0 > 4)
        thismenu_sm0 = 2;
}
static inline void mInformation_menu_fxKey4(void)
{
    thismenu_sm0_last = thismenu_sm0;
    thismenu_sm0 = 5;
}

PTRFX_retVOID const mInformation_menu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mInformation_menu_fxKey0, mInformation_menu_fxKey1, mInformation_menu_fxKey2,
    mInformation_menu_fxKey3, mInformation_menu_fxKey4
};

void mInformation_menu_job(void)
{
    char setting_value[SDCARD_SETTING_VALUE_MAX_LENGTH];

    if (thismenu_sm0 == 0)
    {
        thismenu_sm0_last = 2;
        thismenu_sm0++;
    }
    if (thismenu_sm0 == 1)
    {
        kb_change_keyDo_pgm(mInformation_menu_fxKey);
        lcd.clear();
        sysPrint_lcdPrintPSTR(LCD_ROW_0, 0, PSTR("[INFORMATION]"));
        thismenu_sm1 = 0;
        thismenu_sm0 = thismenu_sm0_last;
    }

    if (thismenu_sm0 == 2 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("Part Number:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("PartNumber"),setting_value) )
                sysPrint_lcdPrint(LCD_ROW_3, STR_CENTER, String(setting_value) );
            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 == 3 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("Serial Number:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("SerialNumber"),setting_value) )
                sysPrint_lcdPrint(LCD_ROW_3, STR_CENTER, String(setting_value) );

            thismenu_sm1 = -1;
        }
    }
    if (thismenu_sm0 == 4 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrint_lcdPrintPSTR(LCD_ROW_2, STR_CENTER, PSTR("Software Version:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("SoftwareVersion"),setting_value) )
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
