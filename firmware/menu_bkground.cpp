#include <Arduino.h>
#include "menu.h"
#include "menu_bkground.h"
#include "system.h"
#include "ikey.h"
#include "kb.h"
#include "vscroll.h"
#include "lcd_fxaux.h"
#include "LTC68042.h"
#include "ltc6804.h"
#include "stackError.h"
#include "getString.h"
#include "sysprint.h"

#include "menu.h"
#include "menu_stackInDetail_V.h"
#include "menu_stackInDetail_E.h"
#include "menu_mainMenu.h"

#include "temperature.h"

#include "sysprint.h"

void board_event(void)
{
    	if (boardD.set_text)//by software (show again on-demand)
		{
			boardD.set_text = 0;
			board_setText();
		}
		if(boardD.AchangeInBoard)//by hardware
		{
			board_setText();

			int8_t current_menu = menu_getSubMenu();
			if (
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_V_JOB ) ||
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_E_JOB)
				)
			{
				vscroll.stack_pop();//1 pop x default
			}
			if (
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_E_UNDERVOLT_JOB) ||
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_E_OVERVOLT_JOB) ||
				(current_menu == MGO_MBKGROUND_STACKINDETAIL_E_OPENWIRE_JOB)
			   )
			{
				vscroll.stack_pop(2);//2 pop
			}
			menu_setSubMenu(MGO_MBKGROUND_SHOWSTACKSUMMARY_JOB);
		}

}

void board_setText(void)
{
    char title[SYSPRINT_STR_MAX_SIZE];
    char buff[SYSPRINT_STR_MAX_SIZE];

    if(num_board == 0)
    {
        //strcpy_P(title, PSTR("[MASTER]"));
        //added 2020
        strcpy_P(title, PSTR("[MSTR]"));
    }
    else
    {
//        strcpy_P(title, PSTR("[EXT. "));
//        itoa(num_board, buff, 10);
//        strcat(title,buff);
//        strcat_P(title, PSTR("]"));

        //added 2020
        strcpy_P(title, PSTR("[EX."));//EXT MAX 1..7
        itoa(num_board, buff, 10);
        strcat(title,buff);
        strcat_P(title, PSTR("]"));
    }

    if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        //title +="[STACK1+2]"
        //strcat_P(title, PSTR("[STACK "));
        strcat_P(title, PSTR("[STK "));//added 2020

        itoa(board[num_board].stack[StackLower].address + 1, buff, 10);
        strcat(title, buff);
        strcat_P(title, PSTR("+"));
        itoa(board[num_board].stack[StackUpper].address + 1, buff, 10);
        strcat(title, buff);
        strcat_P(title, PSTR("]"));
        //
        board[num_board].stacks.num_stack = 0;

        //title [MSTR][STK 15+16]
    }
    else  if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        //S = # de stack
        uint8_t opt = board[num_board].stacks.num_stack;//num_stack siempre entre 0 - 1, de ser el caso q ambos stacks no esten unidos

        if(board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire)
        {
            //title +="[STACK S]";
            //strcat_P(title, PSTR("[STACK "));
            strcat_P(title, PSTR("[STK "));//added 2020
            itoa(board[num_board].stack[opt].address + 1, buff, 10);
            strcat(title, buff);
            strcat_P(title, PSTR("]"));

            //title [MSTR][STK 16]
        }
        else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist)
        {
//
//[MSTR][STK 15+16]
//[MSTR]STK 15+16=E 16
//[MSTR]STACK 9+1=E 10
//[EX.7]STK 15+16=E 16

            //title +="[STACK S+(S+1)] S";//Aqui me muestra que estan conectados solo como texto "1+2", y al costado el stack upper o lower, ya que no se formo' correctamente

            //strcpy_P(title, PSTR("[STACK"));
            strcat_P(title, PSTR("STK "));//added 2020
            itoa(board[num_board].stack[StackLower].address + 1 , buff, 10);
            strcat(title, buff);
            strcat_P(title, PSTR("+"));

            itoa(board[num_board].stack[StackUpper].address + 1 , buff, 10);
            strcat(title, buff);
            strcat_P(title, PSTR("=E "));

            itoa(board[num_board].stack[opt].address + 1 , buff, 10);
            strcat(title, buff);
        }


        if(ltc_connected[ board[num_board].stack[opt].address] == LTC_DISCONNECTED)
        {
            char str[SYSPRINT_STR_MAX_SIZE];

            if(board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist)
            {
                sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("<< CHAINED STACK? >>"));//added 2020
            }
            else
            {
                sysPrintLCDan_XY_wlineClr(LCD_ROW_1,0,"");//clear Line
            }


            //String txt = "STACK " + String(board[num_board].stack[opt].address + 1);
            strcpy_P(str, PSTR("STACK "));
            itoa( board[num_board].stack[opt].address + 1, buff, 10);
            strcat(str, buff);
            sysPrintLCDan_XY_wlineClr_wEOL3dots(LCD_ROW_2, LCDAN_STR_FORMAT_ALIGN_CENTER, str);

            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(LCD_ROW_3, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("NOT CONNECTED"));
        }
    }
    sysPrintLCDan_XY_wlineClr(LCD_ROW_0, 0, title);


     //+-add
    //if (thismenu_sm0 == STACKSUMMARY)
    if (menu_getSubMenu() == MGO_MBKGROUND_SHOWSTACKSUMMARY_JOB)
    {
        sysPrintLCDan_XY(LCD_ROW_0, 18, "<>");
    }
    //-+
}

static void mBkground_showStackSummary_fxKey0(void)//Menu
{
    menu_setMenu(MGO_MAIN_MENU);
    //menu_setSubMenu(-1);
}
static void mBkground_showStackSummary_fxKey1(void)//Up
{
    if(--board[num_board].stacks.num_stack < 0)  //board[num_board].stack.option.limit_inf )
    {
        board[num_board].stacks.num_stack = 0;

        if(--num_board < 0)
        {
            num_board  = NUM_BOARD - 1;

            for(uint8_t nb = 0; nb < NUM_BOARD; nb++)
                board[nb].stacks.num_stack = board[num_board].stacks.num_max_stacks - 1;
        }

        board[num_board].unionMode_last = -1; //provoca un cambio para q se reevalue el modo de stack
    }

    boardD.AchangeInBoard = 1;
}
static void mBkground_showStackSummary_fxKey2(void)//Down
{
    if(++board[num_board].stacks.num_stack > (board[num_board].stacks.num_max_stacks - 1))
    {
        board[num_board].stacks.num_stack = (board[num_board].stacks.num_max_stacks - 1);

        if(++num_board > NUM_BOARD - 1)
        {
            num_board  = 0;

            for(uint8_t nb = 0; nb < NUM_BOARD; nb++)
                board[nb].stacks.num_stack = 0;
        }

        board[num_board].unionMode_last = -1; //provoca un cambio para q se reevalue el modo de stack
    }

    boardD.AchangeInBoard = 1;
}
static void mBkground_showStackSummary_fxKey3(void)//Enter
{
    uint8_t opt = board[num_board].stacks.num_stack;

    if(ltc_connected[ board[num_board].stack[opt].address] == LTC_CONNECTED)
    {
        menu_setSubMenu(MGO_MBKGROUND_INSIDESTACKSUMMARY_JOB);

        sysPrintLCDan_XY(LCD_ROW_0, 18, "  ");;//clear "<>"
    }
}
static void mBkground_showStackSummary_fxKey4(void)//eScape
{
}

PTRFX_retVOID const mBkground_showStackSummary_fxKey[KB_NUM_KEYS] PROGMEM=
{mBkground_showStackSummary_fxKey0, mBkground_showStackSummary_fxKey1, mBkground_showStackSummary_fxKey2, mBkground_showStackSummary_fxKey3, mBkground_showStackSummary_fxKey4};


static void mBkground_showStackSummary_updateDbData(char (*db_stackSummary)[LCDAN_STR_MAX_SIZE]);

#define STACK_SUMMARY_NUM_ELEM 4

void mBkground_showStackSummary_job(void)
{
    static uint8_t doThisJob;

    //String
    char db_stackSummary[STACK_SUMMARY_NUM_ELEM][LCDAN_STR_MAX_SIZE];
    mBkground_showStackSummary_updateDbData(db_stackSummary);

    if (thismenu_sm0 == 0) //init
    {
        kb_change_keyDo_pgm(mBkground_showStackSummary_fxKey);

        thismenu_sm0 = -1;
    }

    if(boardD.AchangeInBoard)
    {
        if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
        {
            vscroll.init(db_stackSummary, STACK_SUMMARY_NUM_ELEM, vscroll_dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_OFF);
            doThisJob = 1;
        }
        else  if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
        {
            //S = # de stack
            uint8_t opt = board[num_board].stacks.num_stack;

            if(ltc_connected[ board[num_board].stack[opt].address] == LTC_CONNECTED)
            {
                vscroll.init(db_stackSummary, STACK_SUMMARY_NUM_ELEM, vscroll_dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_OFF);
                doThisJob = 1;
            }
            else
                doThisJob = 0;
        }
    }

    if(doThisJob)
    {
        vscroll.set_db(db_stackSummary);
        vscroll.job();
        vscroll_lcd_show_disp();

        if(!ltc6804.app.synch.data_invalid)
            mBkground_showStackSummary_updateDbData(db_stackSummary);
    }
}

static void mBkground_showStackSummary_updateDbData(char (*db_stackSummary)[LCDAN_STR_MAX_SIZE])
{
    uint8_t tempError_Lower, tempError_Upper;
    uint8_t num_ic = BOARD_get_num_ic_currentboard();

    char str_dest[LCDAN_STR_MAX_SIZE];//added

    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        //Voltage Stack
        //db_stackSummary[0] = str_clearPrint(getString_voltage(stack_voltage_ifcF[num_ic]), 4);
        getString_voltage(stack_voltage_ifcF[num_ic], str_dest);
        lcdan_str_lineformat_align(&db_stackSummary[0][0],  str_dest, 4);
        //Current Stack
        //db_stackSummary[1] = str_clearPrint(getString_current(stack_currentF[num_ic]), 4);

        getString_current(stack_currentF[num_ic], str_dest);
        lcdan_str_lineformat_align(&db_stackSummary[1][0],  str_dest, 4);

        //Temperature Stack
        float stackTemperatureF = stack_temperatureF[num_ic];

        //if (stackTemperatureF == NTC10K_NO_CONNECTED)
        if ( (stackTemperatureF-2 <= NTC10K_NO_CONNECTED) && ( stackTemperatureF-2 >= (NTC10K_NO_CONNECTED-10)) )
        {
            //db_stackSummary[2] = str_clearPrint("T= NO CONNECTED", 4);
            lcdan_str_lineformat_align_P(&db_stackSummary[2][0],  PSTR("T= NO CONNECTED"), 4);
        }
        //else if (stackTemperatureF == NTC10K_OUT_RANGE)
        else if ( ( (stackTemperatureF-2) <= NTC10K_OUT_RANGE) && ( (stackTemperatureF-2) >= (NTC10K_OUT_RANGE-10) ) )
        {
            //db_stackSummary[2] = str_clearPrint("T= OUT RANGE", 4);
            lcdan_str_lineformat_align_P(&db_stackSummary[2][0],  PSTR("T= OUT RANGE"), 4);
        }
        else
        {
            //db_stackSummary[2] = str_clearPrint(getString_temperature(stack_temperatureF[num_ic]), 4);
            getString_temperature(stack_temperatureF[num_ic], str_dest);
            lcdan_str_lineformat_align(&db_stackSummary[2][0],  str_dest, 4);
        }
        //

        //Errors = #
        //db_stackSummary[3] = str_clearPrint(getString_error("ERROR= ", get_stackError_total(num_ic)), 4);
        getString_error("ERROR= ", get_stackError_total(num_ic), str_dest);
        lcdan_str_lineformat_align(&db_stackSummary[3][0],  str_dest, 4);
    }
    else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        //Voltage Stack 1+2
        float CV = stack_voltage_ifcF[ board[num_board].stack[StackLower].address ] + stack_voltage_ifcF[ board[num_board].stack[StackUpper].address];
        //db_stackSummary[0] = str_clearPrint(getString_voltage(CV), 4);
        getString_voltage(CV, str_dest);
        lcdan_str_lineformat_align(&db_stackSummary[0][0],  str_dest, 4);

        //Current Stack LOWER
        //db_stackSummary[1] = str_clearPrint(getString_current(stack_currentF[board[num_board].stack[StackLower].address]), 4);
        getString_current(stack_currentF[board[num_board].stack[StackLower].address], str_dest);
        lcdan_str_lineformat_align(&db_stackSummary[1][0],  str_dest, 4);

        //Temperature Stack LOWER
        float stackTemperatureF = stack_temperatureF[board[num_board].stack[StackLower].address];
        if (stackTemperatureF == NTC10K_NO_CONNECTED)
        {
            //db_stackSummary[2] = str_clearPrint("T= NO CONNECTED", 4);
            lcdan_str_lineformat_align_P(&db_stackSummary[2][0],  PSTR("T= NO CONNECTED"), 4);
        }
        else if (stackTemperatureF == NTC10K_OUT_RANGE)
        {
            //db_stackSummary[2] = str_clearPrint("T= OUT RANGE", 4);
            lcdan_str_lineformat_align_P(&db_stackSummary[2][0],  PSTR("T= OUT RANGE"), 4);
        }
        else
        {
            //db_stackSummary[2] = str_clearPrint(getString_temperature(stackTemperatureF), 4);
            getString_temperature(stackTemperatureF, str_dest);
            lcdan_str_lineformat_align(&db_stackSummary[2][0],  str_dest, 4);
        }
        //



        //Errors = #
        tempError_Lower = get_stackError_total(board[num_board].stack[StackLower].address);
        tempError_Upper = get_stackError_total(board[num_board].stack[StackUpper].address);

        //db_stackSummary[3] = str_clearPrint(getString_error("ERROR= ", tempError_Lower+tempError_Upper), 4);
        getString_error("ERROR= ", tempError_Lower+tempError_Upper, str_dest);
        lcdan_str_lineformat_align(&db_stackSummary[3][0],  str_dest, 4);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void mBkground_insideStackSummary_fxKey0(void)
{

}
static void mBkground_insideStackSummary_fxKey1(void)
{
    vscroll.sign_up();
}
static void mBkground_insideStackSummary_fxKey2(void)
{
    vscroll.sign_down();
}
static void mBkground_insideStackSummary_fxKey3(void)
{
    vscroll.stack_push();
    vscroll.base_offset_reset();

    switch (vscroll.get_markPosc())
    {
        case 0://V
                menu_setSubMenu(MGO_MBKGROUND_STACKINDETAIL_V_JOB);
            break;

        case 1://I
            break;

        case 2://T
            break;

        case 3://E
            menu_setSubMenu(MGO_MBKGROUND_STACKINDETAIL_E_JOB);
            break;
    }
}
void mBkground_insideStackSummary_fxKey4(void)
{
    menu_setSubMenu(MGO_MBKGROUND_SHOWSTACKSUMMARY_JOB);

    vscroll.feat_mark_off();

    //+-add
    //if (thismenu_sm0 == STACKSUMMARY)
        {lcd.setCursor(18,0);lcd.print("<>");}
    //-+
}

PTRFX_retVOID const mBkground_insideStackSummary_fxKey[KB_NUM_KEYS] PROGMEM=
{mBkground_insideStackSummary_fxKey0, mBkground_insideStackSummary_fxKey1, mBkground_insideStackSummary_fxKey2, mBkground_insideStackSummary_fxKey3, mBkground_insideStackSummary_fxKey4};

void mBkground_insideStackSummary_job(void)
{
    char db_stackSummary[STACK_SUMMARY_NUM_ELEM][LCDAN_STR_MAX_SIZE];

    if(thismenu_sm0 == 0)
    {
        kb_change_keyDo_pgm(mBkground_insideStackSummary_fxKey);

        vscroll.init(db_stackSummary, STACK_SUMMARY_NUM_ELEM, vscroll_dispShowBuff, DISP_SHOW_BUFF_LENGTH, FEAT_MARK_ON, vscroll.base, vscroll.offset);
        //vscroll.feat_mark_on();

        thismenu_sm0 = -1;
    }

    if(!ltc6804.app.synch.data_invalid)
        mBkground_showStackSummary_updateDbData(db_stackSummary);

    vscroll.set_db(db_stackSummary);
    vscroll.job();
    vscroll_lcd_show_disp();

//    if(!ltc6804.app.synch.data_invalid)
//        mBkground_showStackSummary_updateDbData(db_stackSummary);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A U X I L I A R
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t get_db_size(void)
{
    uint8_t db_size = 0;
    uint8_t num_ic;
    uint8_t stack_LU;

    if((board[num_board].unionMode == StackLowerUpper_NO_UnitedByWire) || (board[num_board].unionMode == StackLowerUpper_UnitedByWire_but_LowerNoExist))
    {
        num_ic = BOARD_get_num_ic_currentboard();

        if(select_2_12V[num_ic] == 2)
            db_size = NUM_CELL_2V_MAX;
        else
            db_size = NUM_CELL_12V_MAX;
    }
    else if(board[num_board].unionMode == StackLowerUpper_UnitedByWire)
    {
        db_size=0;
        for (stack_LU=StackLower; stack_LU<=StackUpper; stack_LU++)
        {
            num_ic = board[num_board].stack[stack_LU].address;

            if(select_2_12V[num_ic] == 2)
                db_size += NUM_CELL_2V_MAX;
            else
                db_size += NUM_CELL_12V_MAX;
        }

    }

    return db_size;
}


