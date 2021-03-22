#include <Arduino.h>
#include "system.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "ikey.h"
#include "kb.h"
#include "board.h"
#include "menu.h"

#include "sysprint.h"

static inline void mainMenuSummary_fxKey0(void)
{
}
static inline void mainMenuSummary_fxKey1(void)
{
    vscroll.sign_up();
}
static inline void mainMenuSummary_fxKey2(void)
{
    vscroll.sign_down();
}
static inline void mainMenuSummary_fxKey3(void)
{
    uint8_t posc = vscroll.get_markPosc();

    if ( (posc !=1))// && (posc !=2))
    {
        menu_setMenu(posc+1); //men=[1...8]//men = vscroll.get_markPosc()+1;//pos 0 is for bkground
        menu_setSubMenu(0);                      //submen = 0;
        //
        vscroll.stack_push();
        vscroll.base_offset_reset();
    }

}
static inline void mainMenuSummary_fxKey4(void)
{
    menu_setMenu(MGO_MBKGROUND);//men = 0;
    menu_setSubMenu(MGO_MBKGROUND_SHOWSTACKSUMMARY_JOB);//submen=0
    vscroll.base_offset_reset();
    //
    boardD.AchangeInBoard =1;
    lcd.clear();
}

PTRFX_retVOID const mainMenuSummary_fxKey[KB_NUM_KEYS] PROGMEM =
{mainMenuSummary_fxKey0, mainMenuSummary_fxKey1, mainMenuSummary_fxKey2, mainMenuSummary_fxKey3, mainMenuSummary_fxKey4};

void mainMenuSummary_job(void)
{
    char db_mainMenuSummary[MAIN_MENU_NUMMAX][LCDAN_STR_MAX_SIZE];

    lcdan_str_lineformat_align_P(&db_mainMenuSummary[0][0],  PSTR("1] INPUTS"), 4);
    lcdan_str_lineformat_align_P(&db_mainMenuSummary[1][0],  PSTR("2] OUTPUTS"), 4);
    lcdan_str_lineformat_align_P(&db_mainMenuSummary[2][0],  PSTR("3] LOGICAL FX"), 4);//SYS ERROR
    lcdan_str_lineformat_align_P(&db_mainMenuSummary[3][0],  PSTR("4] INFORMATION"), 4);
    lcdan_str_lineformat_align_P(&db_mainMenuSummary[4][0],  PSTR("5] CONTACT"), 4);
    lcdan_str_lineformat_align_P(&db_mainMenuSummary[5][0],  PSTR("6] SET TIME"), 4);
    lcdan_str_lineformat_align_P(&db_mainMenuSummary[6][0],  PSTR("7] SET DATE"), 4);
    lcdan_str_lineformat_align_P(&db_mainMenuSummary[7][0],  PSTR("8] FUEL SENSOR"), 4);

    if (thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mainMenuSummary_fxKey);

        vscroll.init(db_mainMenuSummary, MAIN_MENU_NUMMAX, vscroll_dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);

        sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_0, 0, PSTR("[MAIN MENU]"));

        thismenu_sm0 = -1;
    }
    vscroll.set_db(db_mainMenuSummary);
    vscroll.job();
    vscroll_lcd_show_disp();
}


