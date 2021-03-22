#ifndef LOGF_H_
#define LOGF_H_

#include "digital_input.h"
#include "analog_input.h"
#include "rele.h"

void logf_init(void);


extern class _logf_digital_input logf_digital_input[NUM_DIG_INPUT_MAX];
extern class _logf_analog_input logf_analog_input[NUM_ANALOG_INPUT_MAX];
extern class _logf_rele logf_rele[NUM_RELE_MAX];

extern class _logf_assig logf_assig;

extern class _Symbol Symbol;
extern class _logf_outAssign logf_outAssign;
extern struct _logf_logExp_assigOut_data logf_logExp_assigOut_data;//deberia de llamarlo a traves del puntero

extern uint8_t num_exp_saved;

void logf_logExpAssig_man(void);

#endif // LOGICBLOCK_H_
