#ifndef SETNUM_H_INCLUDED
#define SETNUM_H_INCLUDED

#define BLINK_TOGGLE_BLANK 0
#define BLINK_TOGGLE_NOBLANK 1

#define SETNUM_CHARNUMBER_MAX_QTY_OF_NUMBERS 3
#define SETNUM_CHARVALUE_MAX_LENGTH (7+1) //7+ '\0'

struct _setnum
{
    struct _digit
    {
        int8_t position;//current

        struct _blink
        {
            uint8_t sm0;
            unsigned long last_millis;
            unsigned long timeout;

            struct _bf
            {
                unsigned toggle:1;
                unsigned __a:7;
            } bf;
        }blink;
    }digit;


    struct
    {
        uint8_t qty_of_charnumber; //in used by app

        struct
        {
            char charvalue[SETNUM_CHARVALUE_MAX_LENGTH];

            struct _digitposition
            {
                uint8_t dp;             //if charvalue place hold float dp>0
                uint8_t endofdigit;           //without consider the '\0'

            } digitposition;

        } thenumber[SETNUM_CHARNUMBER_MAX_QTY_OF_NUMBERS];

        uint8_t selected;   //active number

    } charnumber;

    struct _printxy
    {
        uint8_t (*pFil) (uint8_t);
        uint8_t (*pCol) (uint8_t);
    }printxy;

};
extern struct _setnum setnum;


void setnum_digit_reset_charnumber_reset(void);
void setnum_digit_set_position(int8_t position);
int8_t setnum_digit_get_position(void);
void setnum_digit_blink_set_timeout(unsigned long timeout);
void setnum_charnumber_qty_of_charnumber(uint8_t qty_of_charnumber);
void setnum_charnumber_thenumber_digitposition_set_dp(uint8_t selected, uint8_t digitposition_decimalpoint);
void setnum_charnumber_thenumber_digitposition_set_endofdigit(uint8_t selected, uint8_t digitposition_endofdigit);
void setnum_printxy_register_pFil(uint8_t (*pFil) (uint8_t));
void setnum_printxy_register_pCol(uint8_t (*pCol) (uint8_t));
void setnum_charnumber_thenumber_digitposition_init(uint8_t selected, uint8_t digitposition_decimalpoint, uint8_t digitposition_last);
void setnum_charnumber_thenumber_digitDec(void);
void setnum_charnumber_thenumber_digitInc(void);
void setnum_charnumber_selectDigit_selectNumber(void);
void setnum_digit_blink_set2blank(void);
void setnum_digit_blink_set2noblank(void);
void setnum_digit_blinking(void);
void setnum_digit_blink_print_digit(char c);
void setnum_charnumber_print_charvalue(uint8_t selected);
void setnum_charnumber_print_charvalue_fCurrSelected(void);
void setnum_charnumber_set_charvalue_asDou_aftFillZeros(uint8_t selected, double dnumber, signed char __witdh, unsigned char __prec);
void setnum_charnumber_set_charvalue_asDou_aftFillZeros_fcurrSelected(double dnumber, signed char __witdh, unsigned char __prec);
void setnum_charnumber_set_charvalue_asDou(uint8_t selected, double dnumber, signed char __witdh, unsigned char __prec);
void setnum_charnumber_set_charvalue_asDou_fcurrSelected(double dnumber, signed char __witdh, unsigned char __prec);
double setnum_charnumber_get_charvalue_asDou(uint8_t selected);
double setnum_charnumber_get_charvalue_asDou_fcurrSelected(void);
uint8_t setnum_charnumber_get_currSelected(void);
void setnum_charnumber_set_currSelected(uint8_t selected);


#endif // SETNUM_H_INCLUDED


