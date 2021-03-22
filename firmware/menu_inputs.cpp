#include <Arduino.h>
#include "system.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"

#include "digital_input.h"
#include "analog_input.h"
#include "menu.h"
#include "menu_mainMenu.h"
#include "menu_inputs.h"
#include "setnum.h"
#include "logf_class.h"
#include "logf.h"
#include "menu_logf_config.h"
#include "fuelsensor.h"

#include "sysprint.h"

static inline void mInput_menu_fxKey0(void)
{
}
static inline void mInput_menu_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mInput_menu_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mInput_menu_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0: menu_setSubMenu(MGO_MINPUT_DIGITALINPUT_JOB);break;
        case 1: menu_setSubMenu(MGO_MINPUT_ANALOGINPUT_JOB);break;
        case 2: menu_setSubMenu(MGO_MINPUT_FUELSENSOR_JOB);break;
    }
}
static inline void mInput_menu_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setMenu(MGO_MAIN_MENU);
}

PTRFX_retVOID const mInput_menu_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mInput_menu_fxKey0, mInput_menu_fxKey1, mInput_menu_fxKey2,
    mInput_menu_fxKey3, mInput_menu_fxKey4
};


void mInput_menu_job(void)
{
    #define MENU_INPUT_SUMMARY_NUMOPT 3
    char db_mInput_menu[MENU_INPUT_SUMMARY_NUMOPT][LCDAN_STR_MAX_SIZE];
    lcdan_str_lineformat_align_P(&db_mInput_menu[0][0],  PSTR("1] Digital Input"), 2);
    lcdan_str_lineformat_align_P(&db_mInput_menu[1][0],  PSTR("2] Analog Input"), 2);
    lcdan_str_lineformat_align_P(&db_mInput_menu[2][0],  PSTR("3] Fuel Sensor"), 2);

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mInput_menu_fxKey);
        vscroll.init(db_mInput_menu, MENU_INPUT_SUMMARY_NUMOPT, vscroll_dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        thismenu_sm0++;
    }

    if (thismenu_sm0 == 1)
    {
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[INPUTS]"));
        thismenu_sm0 = -1;
    }

    vscroll.set_db(db_mInput_menu);
    vscroll.job();
    vscroll_lcd_show_disp();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mInput_digitalInput_fxKey0(void)
{
}
static inline void mInput_digitalInput_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mInput_digitalInput_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mInput_digitalInput_fxKey3(void)
{
}
static inline void mInput_digitalInput_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MINPUT_MENU_JOB);
}

PTRFX_retVOID const mInput_digitalInput_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mInput_digitalInput_fxKey0, mInput_digitalInput_fxKey1, mInput_digitalInput_fxKey2,
    mInput_digitalInput_fxKey3, mInput_digitalInput_fxKey4
};

static void mInput_digitalInput_updateDbData(char (*db)[LCDAN_STR_MAX_SIZE])
{
    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];

    for (uint8_t i=0; i<NUM_DIG_INPUT_MAX; i++ )
    {
        strcpy_P(str, PSTR("Dig-Input "));

        itoa(i+1, buff, 10);
        strcat(str, buff);

        strcat_P(str, PSTR(": "));

        itoa(digital_input[i], buff, 10);
        strcat(str, buff);

        //
        lcdan_str_lineformat_align(&db[i][0], str, 2);
    }
}

//static
void mInput_digitalInput_job(void)
{
    char db_mInput_digitalInput[NUM_DIG_INPUT_MAX][LCDAN_STR_MAX_SIZE];

    mInput_digitalInput_updateDbData(db_mInput_digitalInput);

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mInput_digitalInput_fxKey);
        vscroll.init(db_mInput_digitalInput, NUM_DIG_INPUT_MAX, vscroll_dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        thismenu_sm0 = -1;
    }

    sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[DIGITAL INPUTS]"));

    vscroll.set_db(db_mInput_digitalInput);
    vscroll.job();
    vscroll_lcd_show_disp();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline void mInput_analogInput_fxKey0(void)
{
}
static inline void mInput_analogInput_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mInput_analogInput_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mInput_analogInput_fxKey3(void)
{
}
static inline void mInput_analogInput_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MINPUT_MENU_JOB);
}

PTRFX_retVOID const mInput_analogInput_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mInput_analogInput_fxKey0, mInput_analogInput_fxKey1, mInput_analogInput_fxKey2,
    mInput_analogInput_fxKey3, mInput_analogInput_fxKey4
};

static void mInput_analogInput_updateDbData(char (*db)[LCDAN_STR_MAX_SIZE])
{
    char str[LCDAN_STR_MAX_SIZE];
    char buff[LCDAN_STR_MAX_SIZE];

    char char_buff[SETNUM_CHARVALUE_MAX_LENGTH/*DTOSTRF_SIZE_BUF*/];

    for (uint8_t i=0; i<NUM_ANALOG_INPUT_MAX; i++ )
    {
        //str = "An-Input"+ String(i+1) + ": " + String(analog_input[i]) + String(" V");
        //db[i] = str_clearPrint(str,1);
        strcpy_P(str, PSTR("A"));
        itoa(i+1, buff, 10);
        strcat(str, buff);
        strcat_P(str, PSTR("= "));

        float adcvalue_calibrated = logf_analog_input[i].get_adcvalue_calibrated();

//#define SCALEFACTOR_ASCHAR_NUMINTEGERS 5
//#define SCALEFACTOR_ASCHAR_NUMDECIMALS 6

        //strcat(str, dtostrf(analog_input[i], DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) );
        strcat(str, dtostrf(adcvalue_calibrated, SCALEFACTOR_ASCHAR_NUMINTEGERS, SCALEFACTOR_ASCHAR_NUMDECIMALS, char_buff) );
        //strcat_P(str, PSTR(" V") );
        //
        lcdan_str_lineformat_align(&db[i][0], str, 1);
    }
}

//static
void mInput_analogInput_job(void)
{
    char db_mInput_analogInput[NUM_ANALOG_INPUT_MAX][LCDAN_STR_MAX_SIZE];

    mInput_analogInput_updateDbData(db_mInput_analogInput);

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mInput_analogInput_fxKey);
        vscroll.init(db_mInput_analogInput, NUM_ANALOG_INPUT_MAX, vscroll_dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        thismenu_sm0 = -1;
    }

    sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[ANALOG INPUTS]"));

    vscroll.set_db(db_mInput_analogInput);
    vscroll.job();
    vscroll_lcd_show_disp();
}


/////////////////////////////////////////////////////////////
static inline void mInput_fuelSensor_fxKey0(void)
{
}
static inline void mInput_fuelSensor_fxKey1(void)
{
}
static inline void mInput_fuelSensor_fxKey2(void)
{
}
static inline void mInput_fuelSensor_fxKey3(void)
{
}
static inline void mInput_fuelSensor_fxKey4(void)
{
    vscroll.stack_pop();
    menu_setSubMenu(MGO_MINPUT_MENU_JOB);
}
PTRFX_retVOID const mInput_fuelSensor_fxKey[KB_NUM_KEYS] PROGMEM =
{
    mInput_fuelSensor_fxKey0, mInput_fuelSensor_fxKey1, mInput_fuelSensor_fxKey2,
    mInput_fuelSensor_fxKey3, mInput_fuelSensor_fxKey4
};
void mInput_fuelSensor_job(void)
{
    if (thismenu_sm0 == 0)//siempre SOLO cuando se entra!!!!
    {
        kb_change_keyDo_pgm(mInput_fuelSensor_fxKey);

        lcd.clear();
        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[FUEL SENSOR]"));

        thismenu_sm0++;
    }

    char str_dest[SYSPRINT_STR_MAX_SIZE];
    fuelsensor_reading(str_dest);
    sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_1, 0, str_dest);
}


