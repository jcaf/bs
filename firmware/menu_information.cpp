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

#include "sysprint.h"

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
    char setting_value[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];

    if (thismenu_sm0 == 0)
    {
        thismenu_sm0_last = 2;
        thismenu_sm0++;
    }
    if (thismenu_sm0 == 1)
    {
        kb_change_keyDo_pgm(mInformation_menu_fxKey);
        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[INFORMATION]"));
        thismenu_sm1 = 0;
        thismenu_sm0 = thismenu_sm0_last;
    }

    if (thismenu_sm0 == 2 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Part Number:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("PartNumber"),setting_value) )
                sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, setting_value) ;
            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 == 3 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Serial Number:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("SerialNumber"),setting_value) )
                sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, setting_value );

            thismenu_sm1 = -1;
        }
    }
    if (thismenu_sm0 == 4 )
    {
        if ( thismenu_sm1 == 0)
        {
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Software Version:"));
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("SoftwareVersion"),setting_value) )
                sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, setting_value );

            thismenu_sm1 = -1;
        }
    }

    if (thismenu_sm0 == 5)
    {
        int8_t opt_dest;
        uint8_t cod_ret = menu_generic_yesno(PSTR("[MESSAGE]"),PSTR("Exit?"), &opt_dest);
        if (cod_ret != MENU_GUI_EVENT_WAITING_ACTION)
        {
            if (cod_ret == MENU_GUI_EVENT_OPTION_SELECTED)
            {
                if (opt_dest == GENERIC_YESNO_SELECT_YES)
                {
                    //->exit
                    vscroll.stack_pop();
                    menu_setMenu(MGO_MAIN_MENU);

                }
                else//GENERIC_YESNO_SELECT_NO
                {
                    thismenu_sm0 = 1;//
                }
            }
            else if (cod_ret == MENU_GUI_EVENT_ESCAPE)
            {
                thismenu_sm0 = 1;//
            }


        }
    }
}
