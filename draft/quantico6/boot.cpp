#include <Arduino.h>
#include "system.h"
#include "boot.h"
#include "errorsys.h"
#include "ltc6804.h"
//#include <SPI.h>
#include <SD.h>
#include "lcd_fxaux.h"
#include "sdcard.h"

#define cfgfiles_directory "config/"

static void run_test_H10(void);
static int8_t cfgfiles_save_mainvar( String * setting_name, String * setting_value);
//static void cfgfiles_save_mainvar_MAC(String * setting_value);
static void summary_test_H10(void);

struct _cfgfiles
{
    char filename[SDCARD_NAME_FILE_MAX_LENGTH];
    uint8_t num_vars;
    uint8_t count_num_vars;

    uint8_t match_pair;
    //struct _cfgfiles_vars cfgfiles_vars;
}
cfgfiles[]=
{
    {"funcmod.txt",1},//file name, number of variables
    {"setcon.txt",9},
    {"specs.txt",3},
    {"setup.txt",4},
    {"corfact.txt",5},
    {"setacq.txt",6},
};
#define NUM_CFGFILES sizeof(cfgfiles)/sizeof(cfgfiles[0])
//___________________________________________________________________
struct _mainvar
{
    const char setting_name[SDCARD_SETTING_NAME_MAX_LENGTH];
    String setting_value;
    uint8_t match_pair;

} mainvar[]=
{
    //funcmod.txt
    {"FunctionMode","Production"},

    //setcon.txt
    {"MAC","0x90,0xA2,0xDA,0x0E,0x08,0x51"},
    {"Server1IP","192.168.1.43"},
    {"Host","192.168.1.43"},
    {"Directory","/jsondecode.php"},
    {"ApiKeyW","2S25AHPLERPTOKFF"},

    //specs.txt
    {"SerialNumber","40773206"},
};
#define NUM_MAINVARS sizeof(mainvar)/sizeof(mainvar[0])

//___________________________________________________________________
const struct _corfact_vars
{
    const char setting_name[SDCARD_SETTING_NAME_MAX_LENGTH];
    float *var;
} corfact_vars[] PROGMEM=
{
    {"FACTCORR_2V",&FACTCORR_2V},
    {"FACTCORR_12V_C1",&FACTCORR_12V_C1},
    {"FACTCORR_12V_C2",&FACTCORR_12V_C2},
    {"FACTCORR_12V_C3",&FACTCORR_12V_C3},
    {"FACTCORR_12V_C4",&FACTCORR_12V_C4},
};
#define CORFACT_VARS_LENGTH sizeof(corfact_vars)/sizeof(corfact_vars[0])

uint8_t save_corfact_vars(String *setting_name, String *setting_value)
{
    float *pointer;
    char buff[SDCARD_SETTING_VALUE_VARIABLE_MAX_LENGTH];
    uint8_t cod_ret=0;

    for (uint8_t i=0; i<CORFACT_VARS_LENGTH; i++ )
    {
        strcpy_P(buff, (char *) &corfact_vars[i].setting_name);
        if (String(buff) == *setting_name)
        {
            pointer = (float *) pgm_read_word(&(corfact_vars[i].var));
            setting_value->toCharArray(buff, setting_value->length()+1);
            *pointer = strtod(buff, (char**)0);
            cod_ret = 1;
            break;
        }
    }
    return cod_ret;//found?
}
//___________________________________________________________________
const struct _setacq_vars
{
    const char setting_name[SDCARD_SETTING_NAME_MAX_LENGTH];
    float *var;
}setacq_vars[] PROGMEM=
{
    {"shuntVoltFullScale",&shuntVoltFullScale},
    {"shuntRatedCurrent",&shuntRatedCurrent},
    {"uv_2v",&uv_2v},
    {"ov_2v",&ov_2v},
    {"uv_12v_S3C",&uv_12v_S3C},
    {"ov_12v_S3C",&ov_12v_S3C},
};
#define SETACQ_VARS_LENGTH sizeof(setacq_vars)/sizeof(setacq_vars[0])

uint8_t save_setacq_vars(String *setting_name, String *setting_value)
{
    char buff[SDCARD_SETTING_VALUE_VARIABLE_MAX_LENGTH];
    float *pointer;
    uint8_t cod_ret=0;

    for (uint8_t i=0; i<SETACQ_VARS_LENGTH; i++ )
    {
        strcpy_P(buff, (char *) &setacq_vars[i].setting_name);
        if ( String(buff) == *setting_name )
        {
            pointer = (float *) pgm_read_word(&(setacq_vars[i].var));
            setting_value->toCharArray(buff, setting_value->length()+1);
            *pointer= strtod(buff, (char**)0);
            cod_ret = 1;
            break;
        }
    }
    return cod_ret;//found?
}

/********************************************//**
 * @brief Initialize SD card (hardware at low level)
 *
 * @param void
 * @return uint8_t; 1 if successful, else 0
 *
 ***********************************************/
uint8_t sdcard_init(void)
{
    Sd2Card card;
    SdVolume volume;

    if (!card.init(SPI_HALF_SPEED, SDCARD_CS))
    {
        sdcard.lowlevel.error = 1;
        sdcard.card.errorCode = card.errorCode();//+detailed
        return 0;
    }

    /**< volume.init(card) */
    /**<  return The value one, true, is returned for success and*/
    /**<  the value zero, false, is returned for failure.  Reasons for*/
    /**<  failure include not finding a valid partition, not finding a valid*/
    /**<  FAT file system in the specified partition or an I/O error.*/
    if (!volume.init(card))
    {
        sdcard.lowlevel.error = 2;
        return 0;
    }

    sdcard.volume.fatType = volume.fatType();

    if (SD.begin(SDCARD_CS))//call SD.begin ONCE
    {
        sdcard.begin = 1;
    }

    return 1;
}

/********************************************//**
 * @brief Get function mode from SD CARD file "funcmod.txt"
 *          and set the global variable "mysystem.funct_mode"
 * @param void
 * @return uint8_t; 1 if found file, else 0
 *
 ***********************************************/
uint8_t system_set_functionmode(void)
{
    char filepath[SDCARD_PATH_DIR_MAX_LENGTH + SDCARD_NAME_FILE_MAX_LENGTH];
    String setting_name,setting_value;
    struct _variable variable;
    uint8_t cod_ret = 0;

    strcpy(filepath, cfgfiles_directory);
    strcat(filepath, "funcmod.txt");//cfgfiles[0].filename
    File myFile = SD.open(filepath);

    if (myFile)
    {
        while (myFile.available())
        {
            variable = cfgfiles_getName_getValue(&myFile, &setting_name, &setting_value);

            if (variable.bf.sintax)
            {
                if (setting_name == "FunctionMode")
                {
                    if (setting_value == "Production")
                        mysystem.funct_mode = FUNCT_MODE_PRODUCTION;//0
                    else if (setting_value == "Debug")
                        mysystem.funct_mode = FUNCT_MODE_DEBUG;//1
                    else if (setting_value == "Test")
                        mysystem.funct_mode = FUNCT_MODE_TESTING;//2

                    cod_ret = 1;
                }

                break;
            }
        }

        myFile.close();
    }
    return cod_ret;
}

/********************************************//**
 * @brief Try to run "test H10"
 *
 * @param void
 * @return void
 *
 ***********************************************/
void sdcard_runtest_H10(void)
{
    lcd.clear();
    sysPrint_lcd_serialPrintlnPSTR(1,STR_CENTER,PSTR("RUNNING TEST H10"));
    delay(SDCARD_BOOT_LCD_TIME2VIEW*2);

    sysPrint_serialPrintln("");
    lcd.clear();
    sysPrint_lcd_serialPrintlnPSTR(1,STR_CENTER,PSTR("Check SD low-level.."));
    delay(SDCARD_BOOT_LCD_TIME2VIEW);

    lcd.clear();
    sysPrint_lcd_serialPrintlnPSTR(0,STR_CENTER,PSTR("SD CARD"));

    if (sdcard.lowlevel.error == 0)
    {
        sysPrint_lcd_serialPrintln(2,STR_CENTER,"[Format: FAT" + String(sdcard.volume.fatType) +"]");
        delay(SDCARD_BOOT_LCD_TIME2VIEW);
    }
    else if (sdcard.lowlevel.error == 1)
        error_mgm(SDCARD_BOOT_LCD_TIME2VIEW*2, ERROR_LCD_AUTOERASE_OFF,"H10.1", String(sdcard.card.errorCode));
    else if (sdcard.lowlevel.error == 2)
        error_mgm(SDCARD_BOOT_LCD_TIME2VIEW*2, ERROR_LCD_AUTOERASE_OFF,"H10.2");

    if (sdcard.begin)
    {
        if ( SD.open(cfgfiles_directory) == false)//!< in boolean context
        {
            error_mgm(SDCARD_BOOT_LCD_TIME2VIEW*2, ERROR_LCD_AUTOERASE_OFF,"H10.3", String(cfgfiles_directory) );
        }
        else
        {
            run_test_H10();
            summary_test_H10();
        }
    }

    lcd.clear();
    sysPrint_lcd_serialPrintlnPSTR(1,STR_CENTER,PSTR("FINISH TEST H10"));
    delay(SDCARD_BOOT_LCD_TIME2VIEW);
}


/********************************************//**
 * @brief Execute "test H10"
 *
 * @param void
 * @return void
 *
 ***********************************************/
static void run_test_H10(void)
{
    File myFile;
    char filepath[SDCARD_PATH_DIR_MAX_LENGTH + SDCARD_NAME_FILE_MAX_LENGTH];
    String setting_name,setting_value;

    struct _variable variable;
    String msg;

//    myFile.setTimeout(100);

    for (uint8_t f=0; f<NUM_CFGFILES; f++)
    {
        strcpy(filepath, cfgfiles_directory);
        strcat(filepath, cfgfiles[f].filename);

        sysPrint_serialPrintln("");
        lcd.clear();
        sysPrint_lcd_serialPrintln(1,STR_CENTER, "Opening " +String(cfgfiles[f].filename));
        delay(SDCARD_BOOT_LCD_TIME2VIEW);

        myFile = SD.open(filepath);

        if (myFile == false)
        {
            lcd.clear();
            error_mgm(SDCARD_BOOT_LCD_TIME2VIEW*2, ERROR_LCD_AUTOERASE_OFF,"H10.3", String(cfgfiles[f].filename));
        }
        else
        {
            String filename = String(cfgfiles[f].filename);
            filename.toUpperCase();
            sysPrint_lcd_serialPrintln(0,STR_CENTER, filename);
            sysPrint_lcd_serialPrintln(1,STR_CENTER, "Reading variables...");

            cfgfiles[f].match_pair = 1;//!< mark as File read

            while (myFile.available())
            {
                variable = cfgfiles_getName_getValue(&myFile, &setting_name, &setting_value);

                if (variable.bf.sintax)
                {
                    cfgfiles[f].count_num_vars++;

                    if (variable.bf.lcdprint)
                    {
                        sysPrint_lcdPrint(2,0,"["+ setting_name +"]");
                        sysPrint_lcdPrint(3,0,"["+ setting_value +"]");

                        sysPrint_serialPrintln("["+ setting_name +"]=["+ setting_value +"]");

                        delay(SDCARD_BOOT_LCD_TIME2VIEW);
                    }

                    if (cfgfiles_save_mainvar(&setting_name, &setting_value) == 1)//!< if matching pair
                    {
                        sysPrint_serialPrintln("<<< "+ setting_name + " IS MAIN VARIABLE >>>");
                    }


                    if ( strcmp(cfgfiles[f].filename, "corfact.txt") == 0)
                        {save_corfact_vars(&setting_name, &setting_value);}

                    if ( strcmp(cfgfiles[f].filename, "setacq.txt") == 0)
                        {save_setacq_vars(&setting_name, &setting_value);}

                }
            }

            myFile.close();

            sysPrint_serialPrintln("");
            lcd.clearRows4x20(1);
            sysPrint_lcd_serialPrintln(2,0,"[Vars found:"+ String(cfgfiles[f].count_num_vars) +"/"+ String(cfgfiles[f].num_vars) +"]");

            if (cfgfiles[f].count_num_vars == cfgfiles[f].num_vars)//!< verified numbers of variables
                sysPrint_lcd_serialPrintlnPSTR(3,0,PSTR("[Test: PASS]"));
            else
                sysPrint_lcd_serialPrintlnPSTR(3,0,PSTR("[Test: Missing var]"));

            delay(SDCARD_BOOT_LCD_TIME2VIEW);
        }
    }
}


/********************************************//**
 * @brief Generate summary of "test H10"
 *
 * @param void
 * @return void
 *
 ***********************************************/
static void summary_test_H10(void)
{
    String msg;

    sysPrint_serialPrintln("");
    lcd.clear();

    sysPrint_lcd_serialPrintlnPSTR(1,STR_CENTER, PSTR("Generating"));
    sysPrint_lcd_serialPrintlnPSTR(2,STR_CENTER, PSTR("Summary test H10"));

    delay(SDCARD_BOOT_LCD_TIME2VIEW*2);

    lcd.clear();
    sysPrint_lcd_serialPrintlnPSTR(0,STR_CENTER, PSTR("SUMMARY TEST H10"));

    //!< Total Files
    uint8_t count_total_files=0;

    for (uint8_t f=0; f<NUM_CFGFILES; f++)
        if (cfgfiles[f].match_pair == 1)
            count_total_files++;

    sysPrint_lcd_serialPrintln(1,0, "Files found:" + String(count_total_files) +"/"+ String(NUM_CFGFILES));

    //!< Total Vars
    uint8_t count_total_vars=0;
    uint8_t num_total_vars=0;

    for (uint8_t f=0; f<NUM_CFGFILES; f++)
    {
        count_total_vars +=cfgfiles[f].count_num_vars;
        num_total_vars +=cfgfiles[f].num_vars;
    }

    sysPrint_lcd_serialPrintln(2,0, "Vars found:" + String(count_total_vars) +"/"+ String(num_total_vars));

    //!< Total Main Variables
    uint8_t counter_mainvars=0;
    for (uint8_t i=0; i<NUM_MAINVARS; i++)
        if (mainvar[i].match_pair)
            counter_mainvars++;

    sysPrint_lcd_serialPrintln(3,0,"MainVars found:" + String(counter_mainvars) +"/"+ String(NUM_MAINVARS));

    delay(SDCARD_BOOT_LCD_TIME2VIEW*6);



    //!< Report Total Files - error/missing
    if (count_total_files != NUM_CFGFILES)
    {
        sysPrint_serialPrintln("");
        lcd.clear();
        sysPrint_lcd_serialPrintlnPSTR(0,STR_CENTER,PSTR("REPORT: FILES"));

        for (size_t i=0; i< NUM_CFGFILES; i++)
        {
            if (!cfgfiles[i].match_pair)
            {
                error_mgm(SDCARD_BOOT_LCD_TIME2VIEW*2, ERROR_LCD_AUTOERASE_OFF,"H10.3", String(cfgfiles[i].filename));
                sysPrint_serialPrintln("");
            }
        }
    }

    //!< Report Total Vars - error/missing
    if (count_total_vars != num_total_vars)
    {
        sysPrint_serialPrintln("");
        lcd.clear();
        sysPrint_lcd_serialPrintlnPSTR(0,STR_CENTER,PSTR("REPORT: VARIABLES"));

        error_mgm(SDCARD_BOOT_LCD_TIME2VIEW*2, ERROR_LCD_AUTOERASE_OFF,"H10.4", String("No Main vars"));
        sysPrint_serialPrintln("");

        //        for (size_t i=0; i< num_total_vars; i++)
        //        {
        //           error_mgm(SDCARD_BOOT_LCD_TIME2VIEW*2, ERROR_LCD_AUTOERASE_OFF,"H10.4", String(cfgfiles[i].filename));
        //           sysPrint_serialPrintln("");
        //
        //        }

    }

    //!< Report Total Main Variables - error/missing
    if (counter_mainvars != NUM_MAINVARS)
    {
        sysPrint_serialPrintln("");
        lcd.clear();
        sysPrint_lcd_serialPrintlnPSTR(0,STR_CENTER,PSTR("REPORT: MAIN VARS"));

        for (size_t i=0; i< NUM_MAINVARS; i++)
        {
            if (!mainvar[i].match_pair)
            {
                error_mgm(SDCARD_BOOT_LCD_TIME2VIEW*2, ERROR_LCD_AUTOERASE_OFF,"H10.4", String(mainvar[i].setting_name));
                sysPrint_serialPrintln("");
            }
        }
    }
}

/********************************************//**
 * @brief Open config files, read variable (name/value) and set to global variables
 *      - check syntax and maximum length
 * @param[out] myFile File*; Pointer to opened file
 * @param[out] setting_name String*; pointer to string to write name of variable
 * @param[out] setting_value String*; pointer to string to write value of variable
 * @return struct _variable; field bit
 *
 ***********************************************/
//static
struct _variable cfgfiles_getName_getValue(File *myFile, String * setting_name, String * setting_value)
{
#define cfgfiles_VARS_LENGTH_MAX 30//!< x safe
    char buff[cfgfiles_VARS_LENGTH_MAX+20];

    struct _variable variable = {0};//!< all members to zero
    size_t var_length_max;
    int8_t sm0;
    //char c;
    variable.bf.sintax = 1;
    variable.bf.lcdprint = 1;//!< x defect all variables are printable on LCD

    sm0=0x00;

    if (sm0 == 0)
    {
        if (myFile->find('['))
            sm0++;
        else
            sm0=-1;
    }
    if (sm0 ==1)
    {
        var_length_max = myFile->readBytesUntil('=', buff, cfgfiles_VARS_LENGTH_MAX+20);//!< readBytesUntil is more safe than myFile.readStringUntil('=')

        if (var_length_max == cfgfiles_VARS_LENGTH_MAX+20)//!< error
            sm0 = -1;
        else
        {
            buff[var_length_max]='\0';
            *setting_name = String(buff);
            sm0++;
        }
    }
    if (sm0 ==2)
    {
        var_length_max = myFile->readBytesUntil(']', buff, cfgfiles_VARS_LENGTH_MAX+20);

        if (var_length_max == cfgfiles_VARS_LENGTH_MAX+20)//!< error
            sm0 = -1;
        else
        {
            buff[var_length_max]='\0';
            *setting_value= String(buff);
            sm0++;
        }
    }

    if (sm0 == -1)//!< Error
    {
        variable.bf.sintax = 0;
    }
    return variable;
}


/********************************************//**
 * @brief Save main variables if is founded
 *
 * @param[in] setting_name String*; pointer to string to read name of variable
 * @param[in] setting_value String*; pointer to string to read value of variable
 * @return int8_t
 *        -1: Main var is not found OR var is not a Main-variable
 *         0:Main var exist and was read(was matched)
 *         1:Main var exist and is matching

 ***********************************************/
static int8_t cfgfiles_save_mainvar(String * setting_name, String * setting_value)
{
    int8_t cod_ret = -1;

    int8_t index = mainvar_isFound(*setting_name);
    if (index > -1)
    {
        if (mainvar[(size_t)index].match_pair == 0)
        {
            mainvar[(size_t)index].match_pair = 1;
            mainvar[(size_t)index].setting_value = *setting_value;
        }
        cod_ret  = mainvar[(size_t)index].match_pair;
    }
    return cod_ret;
}


/********************************************//**
 * @brief Found main variable
 *
 * @param setting_name String; is the "search key"
 * @return int8_t; -1 if var not found, else, return the index >=0
 *
 ***********************************************/
int8_t mainvar_isFound(String setting_name)
{
    char setting_name_buff[SDCARD_SETTING_NAME_MAX_LENGTH];
    int8_t index = -1;

    for (size_t i=0; i<NUM_MAINVARS; i++)
    {
        setting_name.toCharArray(setting_name_buff, setting_name.length()+1 );

        if ( strcmp(mainvar[i].setting_name, setting_name_buff ) == 0 )
        {
            index = i;
            break;
        }
    }
    return index;
}

/********************************************//**
 * @brief Main variable get value (global struct mainvar)
 *          find trough the setting name
 * @param setting_name String; is the "search key"
 * @return String; return the string get value if found, else return ""
 *
 ***********************************************/
String mainvar_getvalue(String setting_name)
{
    String setting_value="";

    int8_t index = mainvar_isFound(setting_name);
    if (index > -1)
        setting_value = mainvar[(size_t)index].setting_value;

    return setting_value;
}

/********************************************//**
 * @brief Main variable set value (global struct mainvar)
 *          find trough the setting name
 * @param setting_name String;
 * @param setting_value String;
 * @return uint8_t; 1 if main var found, else 0
 *
 ***********************************************/
uint8_t mainvar_setvalue(String setting_name, String setting_value)
{
    uint8_t mainvar_found = 0;

    int8_t index = mainvar_isFound(setting_name);
    if (index > -1)
    {
        mainvar[(size_t)index].setting_value = setting_value;
        mainvar_found = 1;
    }

    return mainvar_found;
}


