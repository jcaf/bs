#ifndef BOOT_H_
#define BOOT_H_
#include "LTC68042.h"
#include "sdcard.h"
#include <SD.h>

    #define SDCARD_BOOT_LCD_TIME2VIEW 0//0//500//1500//1.5s

    uint8_t sdcard_init(void);
    void sdcard_runtest_H10(void);
    uint8_t system_set_functionmode(void);

    int8_t mainvar_isFound(const char *setting_name);
    void mainvar_getvalue(const char *setting_name, char *setting_value_dest);
    uint8_t mainvar_setvalue(const char *setting_name, const char *setting_value);

enum _FACTCORR{
    FCOR_2V = 0,
    FCOR_12V_C1,
    FCOR_12V_C2,
    FCOR_12V_C3,
    FCOR_12V_C4,

};
enum _SETACQ
{
    SHUNT_VOLT_FULLSCALE = 0,
    SHUNT_RATED_CURRENT,
    UV_2V,
    OV_2V,
    UV_12V_S3C,
    OV_12V_S3C
};

struct _variable cfgfiles_getName_getValue(File *myFile, char *setting_name_dest, char *setting_value_dest);

struct _corfact_shunt
{
    float multiplier;
    float adder;
};
struct _corfact_temperature
{
    //float multiplier;
    float adder;
};


struct _corfact_vars
{
    const char setting_name[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
    float value;
};


struct _setacq_vars
{
    const char setting_name[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
    float value;
};

//extern struct _setacq_vars setacq_vars[6];
//extern struct _corfact_vars corfact_vars[5];

extern struct _setacq_vars setacq_vars[];
extern struct _corfact_vars corfact_vars[];

extern struct _corfact_shunt corfact_shunt;

extern  const char corfact_temperature_names[NUM_IC][SDCARD_STR_NAME_PROPERTIES_MAX_SIZE] PROGMEM;
extern struct _corfact_temperature corfact_temperature[];
#endif // BOOT_H_
