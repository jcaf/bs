#ifndef MYEEPROM_H_INCLUDED
#define MYEEPROM_H_INCLUDED

#include "fuelsensor.h"
#include "spline.h"
#include "logf_class.h"
#include "analog_input.h"

#define LOGEXP_EEPROM_NUM_EXP_SAVED (20)
#define EEPROM_ADDR_LOGF_EXP_NUM_EXP_SAVED (sizeof(struct _fsEEPROM) + sizeof(struct _splEE) )
#define EEPROM_ADDR_LOGF_LOGEXP_ASSIGOUT_DATA (EEPROM_ADDR_LOGF_EXP_NUM_EXP_SAVED + sizeof(uint8_t) )

#define EEPROM_ADDR_ANINPUT_FACTORS_DATA (EEPROM_ADDR_LOGF_LOGEXP_ASSIGOUT_DATA + (LOGEXP_EEPROM_NUM_EXP_SAVED* sizeof(struct _logf_logExp_assigOut_data)))

#define EEPROM_ADDR_RELE_BUSY (EEPROM_ADDR_ANINPUT_FACTORS_DATA + (sizeof(struct _an_input_factors)*NUM_ANALOG_INPUT_MAX ) )

#endif // MYEEPROM_H_INCLUDED
