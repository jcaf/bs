#include <Arduino.h>
#include "system.h"
#include "menu_contactInfo.h"

#include "LTC68042.h"
#include "ltc6804.h"

/*
void menu_contactInfo_handler(void)
{
    static int8_t sm0=0;

    if (sm0 == 0)
    {
        lcd.print(F("[CONTACT INFO.]"));

        sm0++;
    }
}

//local
void menu_contactInfo_db_update_data(void)
{
    uint8_t num_ic=1;
    char char_buff[DTOSTRF_SIZE_BUF];

    float cv;
    String str;
    for (uint8_t num_cell=0; num_cell<12; num_cell++)
    {
        cv= vcellf[num_ic][num_cell];
        str="A" + String(num_cell+1) + "=" + dtostrf(cv, DTOSTRF_WIDTH, DTOSTRF_PREC, char_buff) + "R";

        db_global[num_cell]=str_format(str, 4);
    }
}

*/
