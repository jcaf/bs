/****************************************************************************
* Project: Battery System
*
* 2016-09-13-17.13
* jcaf @ jcafOpenSuse
*
* Copyright 2013 Juan Carlos Agüero Flores
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
****************************************************************************/
#include <Arduino.h>
#include "logf_class.h"

#include "logf.h"
//////////////////////////////////////////////////////////////////////////////////////
void _logf_digital_input::set_addr_fx_read_pin(PTRFX_retUINT8_T addr_read_pin)
{
    fx_read_pin = addr_read_pin;
}

void _logf_digital_input::set_logic(uint8_t l)
{
    logic = l;
}

uint8_t _logf_digital_input::get_logic(void)
{
    return logic;
}

uint8_t _logf_digital_input::get_pinvalue_internal(void)
{
    return fx_read_pin();
}

uint8_t _logf_digital_input::get_pinvalue_withlogic(void)
{
    return (logic == 1)? fx_read_pin(): !fx_read_pin();
}

//////////////////////////////////////////////////////////////////////////////////////
void _logf_analog_input::set_addr_fx_read_pin(PTRFX_retUINT16_T addr_fx_read_pin)
{
    fx_read_pin = addr_fx_read_pin;
}

void _logf_analog_input::set_scale_factor(double sf)
{
    an_input_factors.scale = sf;
}
void _logf_analog_input::set_offset(double o)
{
    an_input_factors.offset = o;
}
void _logf_analog_input::set_units(uint8_t u)
{
    an_input_factors.units = u;
}
double _logf_analog_input::get_scale_factor(void)
{
    return an_input_factors.scale;
}
double _logf_analog_input::get_offset(void)
{
    return an_input_factors.offset;
}
uint8_t _logf_analog_input::get_units(void)
{
    return an_input_factors.units;
}

uint16_t _logf_analog_input::get_adcvalue_internal(void)
{
    return fx_read_pin();
}

double _logf_analog_input::get_adcvalue_calibrated(void)
{
    return (fx_read_pin() * an_input_factors.scale) + an_input_factors.offset;
}
//////////////////////////////////////////////////////////////////////////////

void _logf_rele::set_addr_fx_write_pin(PTRFX_retVOID_arg1_UINT8_T addr_fx_write_pin)
{
    fx_write_pin = addr_fx_write_pin;
}
void _logf_rele::set_value(uint8_t value)
{
    fx_write_pin(value);
}

void _logf_rele::set_addr_fx_read_pin(PTRFX_retUINT8_T addr_fx_read_pin)
{
    fx_read_pin = addr_fx_read_pin;
}
uint8_t _logf_rele::get_pinvalue(void)
{
    return fx_read_pin();
}

//////////////////////////////////////////////////////////////////////////////////////
void _logf_symbol::set_valui(uint8_t val)
{
    valui = val;
}
uint8_t _logf_symbol::get_valui(void) const
{
    return valui;
}
void _logf_symbol::set_valf(float val)
{
    valf = val;
}
float _logf_symbol::get_valf(void) const
{
    return valf;
}
void _logf_symbol::set_type(int8_t typ)
{
    type  = typ;
}
int8_t _logf_symbol::get_type(void) const
{
    return type;
}
void _logf_symbol::set_subtype(int8_t subtyp)
{
    subtype  = subtyp;
}
int8_t _logf_symbol::get_subtype(void) const
{
    return subtype;
}

//////////////////////////////////////////////////////////////////////////////////////
void _logf_output::set_type(int8_t typ)
{
    type = typ;
}
uint8_t _logf_output::get_type(void) const
{
    return type;
}
void _logf_output::set_subtype(int8_t subtyp)
{
    subtype = subtyp;
}
uint8_t _logf_output::get_subtype(void) const
{
    return subtype;
}
void _logf_output::set_rele(uint8_t r)
{
    rele = r;
}
void _logf_output::set_rele_activeif(uint8_t ai)
{
    rele_activeif = ai;
}
uint8_t _logf_output::get_rele(void)
{
    return rele;
}
uint8_t _logf_output::get_rele_activeif(void)
{
    return rele_activeif;
}
void _logf_output::set_valf(float _valf)
{
    valf = _valf;
}
float _logf_output:: get_valf(void)
{
    return valf;
}

//////////////////////////////////////////////////////////////////////////////////////
uint8_t _logf_outAssign::add(const class _logf_output &logf_output)
{
    uint8_t cod_ret=0;
    if (pdata->outAssign_ptr < LOGEXP_OUTPUT_NUM_ELEMENTS-1)
    {
        pdata->outAssign_arr[pdata->outAssign_ptr] = logf_output;

        pdata->outAssign_ptr++;
        cod_ret = 1;
    }
    return cod_ret;//1:was space, 0: full buffer
}

uint8_t _logf_outAssign::replace(const class _logf_output &logf_output)
{
    uint8_t cod_ret=0;
    if (pdata->outAssign_ptr < LOGEXP_OUTPUT_NUM_ELEMENTS-1)
    {
        pdata->outAssign_arr[pdata->outAssign_ptr] = logf_output;

        pdata->outAssign_ptr++;
        cod_ret = 1;
    }
    return cod_ret;//1:was space, 0: full buffer
}
uint8_t _logf_outAssign::insert(const class _logf_output &logf_output)
{
    return 1;
}


void _logf_outAssign::clear(void)
{
    class _logf_output logf_output;//default constructor LOGF_ASSIG_TYPE_EOF

    for (uint8_t i=0; i<LOGEXP_OUTPUT_NUM_ELEMENTS; i++)
    {
        pdata->outAssign_arr[i] = logf_output;
    }
    pdata->outAssign_ptr = 0x00;
}

uint8_t _logf_outAssign::get_length(void)
{
    uint8_t length=0;
    while (pdata->outAssign_arr[length].get_type() != LOGF_ASSIG_TYPE_EOV)
    {
        length++;
    }
    return length;
}

uint8_t _logf_outAssign::get_array_rele_available(uint8_t *rele_available)
{
    uint8_t idx = 0;
    for (uint8_t i=0; i<NUM_RELE_MAX; i++)
    {
        if (!logf_rele[i].bf.busy)
        {
            rele_available[idx] = i;
            idx++;
        }
    }
    return idx;//number of rele available
}

uint8_t _logf_outAssign::get_num_rele_available(void)
{
    uint8_t counter = 0;
    for (uint8_t i=0; i<NUM_RELE_MAX; i++)
        if (!logf_rele[i].bf.busy)
            counter++;

    return counter;
}
void _logf_outAssign::set_data_addr(struct _logf_logExp_assigOut_data *_pdata)
{
    pdata = _pdata;
}

void _logf_outAssign::get_text(uint8_t el, char *text_out)
{
    uint8_t rele = pdata->outAssign_arr[el].get_rele();
    uint8_t rele_activeif = pdata->outAssign_arr[el].get_rele_activeif();
    char buff[LCDAN_STR_MAX_SIZE];

    switch (pdata->outAssign_arr[el].get_type())
    {
        case LOGF_ASSIG_TYPE_DIRECT_RELAY:

            //->R1[AiT]
            strcpy_P(text_out,  PSTR("->R"));
            itoa(rele+1, buff, 10);
            strcat(text_out, buff);
            if (rele_activeif)
                strcat_P(text_out,  PSTR("[AiT]"));
            else
                strcat_P(text_out,  PSTR("[AiF]"));

            break;

        case LOGF_ASSIG_TYPE_FX:

            //->Tmr(R1[AiF],36000)//don't show decimals
            switch (pdata->outAssign_arr[el].get_subtype())
            {
                case LOGF_ASSIG_SUBTYPE_FX_TIMER:

                    strcpy_P(text_out,  PSTR("->Tmr(R"));

                    itoa(rele+1, buff, 10);
                    strcat(text_out, buff);
                    if (rele_activeif)
                        strcat_P(text_out,  PSTR("[AiT],"));
                    else
                        strcat_P(text_out,  PSTR("[AiF],"));


                    float valf = pdata->outAssign_arr[el].get_valf();
                    dtostrf(valf,5, 0,buff);
                    strcat(text_out,buff);
                    strcat(text_out,")");
                    break;
            }
            break;

        default:
            break;
    }
}

void _logf_outAssign::incptr(void)
{
    if (++(pdata->outAssign_ptr) > LOGEXP_OUTPUT_NUM_ELEMENTS-2)//-1(EOV) -1
    {
        pdata->outAssign_ptr = LOGEXP_OUTPUT_NUM_ELEMENTS-2;
    }
}
void _logf_outAssign::decptr(void)
{
    if (--(pdata->outAssign_ptr) < 0 )
    {
        pdata->outAssign_ptr = 0;
    }
}
int8_t _logf_outAssign::getptr(void)
{
    return pdata->outAssign_ptr;
}
void _logf_outAssign::setptr(int8_t ptr)
{
    pdata->outAssign_ptr = ptr;
}

//////////////////////////////////////////////////////////////////////////////////////
uint8_t _Symbol::add(const class _logf_symbol &symbol )
{
    uint8_t cod_ret = 0;

    if (pdata->logExp_ptr < LOGEXP_SYMBOL_NUM_ELEMENTS-1)//-1 'cause EOV
    {
        pdata->logExp_arr[pdata->logExp_ptr] = symbol;

        pdata->logExp_ptr++;//inc ptr
//pdata->logExp_length++;//inc length //Fix: remove instruction

        cod_ret = 1;//1:free space, 0: full buffer
    }
    return cod_ret;
}
uint8_t _Symbol::replace(const class _logf_symbol &symbol )//-1 'cause EOV
{
    uint8_t cod_ret = 0;

    if (pdata->logExp_ptr < LOGEXP_SYMBOL_NUM_ELEMENTS-1)
    {
        pdata->logExp_arr[pdata->logExp_ptr] = symbol;

        pdata->logExp_ptr++;//inc ptr

        cod_ret = 1;//1:free space, 0: full buffer
    }
    return cod_ret;
}
uint8_t _Symbol::insert(const class _logf_symbol &symbol )
{
    uint8_t cod_ret = 0;

    return cod_ret;
}
void _Symbol::incptr(void)
{
    if (++(pdata->logExp_ptr) > LOGEXP_SYMBOL_NUM_ELEMENTS-2)
    {
        pdata->logExp_ptr = LOGEXP_SYMBOL_NUM_ELEMENTS-2;
    }
}
void _Symbol::decptr(void)
{
    if ( (--(pdata->logExp_ptr)) < 0 )
    {
        pdata->logExp_ptr = 0;
    }
}
int8_t _Symbol::getptr(void)
{
    return pdata->logExp_ptr;
}
void _Symbol::setptr(int8_t ptr)
{
    pdata->logExp_ptr = ptr;
}

void _Symbol::clear(void)
{
    class _logf_symbol symbol;//llamar al constructor por defecto con LOGF_SYMB_TYPE_EOF

    for (uint8_t i=0; i<LOGEXP_SYMBOL_NUM_ELEMENTS; i++)
    {
        pdata->logExp_arr[i] = symbol;
    }
    pdata->logExp_ptr = 0x00;
    pdata->logExp_length = 0x00;//no used
}

void _Symbol::set_data_addr(struct _logf_logExp_assigOut_data *_pdata)
{
    pdata = _pdata;
}

uint8_t _Symbol::get_length(void)
{
    uint8_t length=0;
    while (pdata->logExp_arr[length].get_type() != LOGF_SYMB_TYPE_EOV)
    {
        length++;
    }
    return length;
}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
uint8_t _Symbol::fx2OP_logical_op_boolean(uint8_t lval, uint8_t op, uint8_t rval)
{
    uint8_t result;

    switch (op)
    {
        case LOGF_LOGICOP_AND:
            result = lval && rval;
            break;
        case LOGF_LOGICOP_OR:
            result = lval || rval;
            break;
        //                case LOGF_LOGICOP_XOR:
        //                    result = lval != rval;
        //                    break;
        default:
            break;
    }
    return result;
}

uint8_t _Symbol::fx2OP_relational_op_boolean(uint8_t lval, uint8_t op, uint8_t rval)
{
    uint8_t result;

    switch (op)
    {
        case LOGF_RELAT_OP_EQUAL_TO:
            result = lval == rval;
            break;
        case LOGF_RELAT_OP_NOTEQUAL_TO:
            result = lval != rval;
            break;
        case LOGF_RELAT_OP_LESSTHAN:
            result = lval < rval;
            break;
        case LOGF_RELAT_OP_GREATERTHAN:
            result = lval > rval;
            break;
        case LOGF_RELAT_OP_LESSTHAN_EQTO:
            result = lval <= rval;
            break;
        case LOGF_RELAT_OP_GREATTHAN_EQTO:
            result = lval >= rval;
            break;
        default:
            break;
    }
    return result;
}
uint8_t _Symbol::fx2OP_algebra_op_boolean(uint8_t lval, uint8_t op, uint8_t rval)
{
    uint8_t result;

    switch (op)
    {
        case LOGF_ARITH_OP_ADD:
            result = lval + rval;
            if (result>1)
                result = 1;
            break;
        case LOGF_ARITH_OP_SUB://prohibido x semantica
            result = lval - rval; //-1 is !0 -> 1
            if (result<0)
                result = 1;
            break;
        case LOGF_ARITH_OP_MUL:
            result = lval * rval;
            break;
        case LOGF_ARITH_OP_DIV://prohibido por semantica
            result = (rval > 0) ? (lval / rval) : lval;
            break;
        default:
            break;
    }

    return result;
}
uint8_t _Symbol::fx2OP_relational_op_float(float lval, uint8_t op, float rval)
{
    uint8_t result;

    switch (op)
    {
        case LOGF_RELAT_OP_EQUAL_TO:
            result = lval == rval;
            break;
        case LOGF_RELAT_OP_NOTEQUAL_TO:
            result = lval != rval;
            break;
        case LOGF_RELAT_OP_LESSTHAN:
            result = lval < rval;
            break;
        case LOGF_RELAT_OP_GREATERTHAN:
            result = lval > rval;
            break;
        case LOGF_RELAT_OP_LESSTHAN_EQTO:
            result = lval <= rval;
            break;
        case LOGF_RELAT_OP_GREATTHAN_EQTO:
            result = lval >= rval;
            break;
        default:
            break;
    }
    return result;
}
float _Symbol::fx2OP_algebra_op_floats(float lval, uint8_t op, float rval)
{
    float result;

    switch (op)
    {
        case LOGF_ARITH_OP_ADD:
            result = lval + rval;
            break;
        case LOGF_ARITH_OP_SUB:
            result = lval - rval;
            break;
        case LOGF_ARITH_OP_MUL:
            result = lval * rval;
            break;
        case LOGF_ARITH_OP_DIV://prohibido por semantica div por 0
            result = (rval > 0) ? (lval / rval) : lval;
            break;
        default:
            break;
    }
    return result;
}


//float->analog,  constantes, voltage, corriente, temperatura, fuel level
//uint8_t->dig, rele, flags //a estos solo se les puede aplicar NOT (!)
class _logf_symbol _Symbol::fx2OP(const class _logf_symbol *lval, const class _logf_symbol *op, const class _logf_symbol *rval)
{
    class _logf_symbol result;

    uint8_t lvalui, rvalui;
    float lvalf, rvalf;

    uint8_t resultui;
    float resultf;

    uint8_t lval_type       = lval->get_type();
    uint8_t lval_subtype    = lval->get_subtype();

    uint8_t rval_type       = rval->get_type();
    uint8_t rval_subtype    = rval->get_subtype();

    uint8_t op_subtype      = op->get_subtype();
    uint8_t op_valui        = op->get_valui();
    void *pval;


    if ((lval_type == LOGF_SYMB_TYPE_OPRND_BOOLEAN) && (rval_type == LOGF_SYMB_TYPE_OPRND_BOOLEAN))
    {
        if (lval_subtype == LOGF_SYMB_SUBTYPE_DIG_INPUT)
        {
            pval = lval->pval;
            lvalui = (uint8_t)(  ((class _logf_digital_input *)pval)->get_pinvalue_withlogic()   );
        }
        else if (lval_subtype == LOGF_SYMB_SUBTYPE_RELAY)
        {
            pval = lval->pval;
            lvalui = (uint8_t)( ((class _logf_rele *)pval)->get_pinvalue()  );
        }
        else if (lval_subtype == LOGF_SYMB_SUBTYPE_FLAGS)
        {
        }
        else if (lval_subtype == LOGF_SYMB_SUBTYPE_RESULT)
        {
            lvalui = lval->get_valui();
        }
        //
        if (rval_subtype == LOGF_SYMB_SUBTYPE_DIG_INPUT)
        {
            pval = rval->pval;
            rvalui = (uint8_t)(  ((class _logf_digital_input *)pval)->get_pinvalue_withlogic()   );
        }
        else if (rval_subtype == LOGF_SYMB_SUBTYPE_RELAY)
        {
            pval = rval->pval;
            rvalui = (uint8_t)( ((class _logf_rele *)pval)->get_pinvalue()  );
        }
        else if (rval_subtype == LOGF_SYMB_SUBTYPE_FLAGS)
        {
        }
        else if (rval_subtype == LOGF_SYMB_SUBTYPE_RESULT)
        {
            rvalui = rval->get_valui();
        }
        //
        switch (op_subtype)
        {
            case LOGF_SYMB_SUBTYPE_LOGICAL_OP:
                resultui = fx2OP_logical_op_boolean(lvalui, op_valui, rvalui);
                break;
            case LOGF_SYMB_SUBTYPE_RELATIONAL_OP:
                resultui = fx2OP_relational_op_boolean(lvalui, op_valui, rvalui);
                break;
            case LOGF_SYMB_SUBTYPE_ARITHMETIC_OP:
                resultui = fx2OP_algebra_op_boolean(lvalui, op_valui, rvalui);
                break;
            default:
                break;

        }
        result.set_valui(resultui);
        result.set_type(LOGF_SYMB_TYPE_OPRND_BOOLEAN);
        //result.set_subtype(LOGF_SYMB_SUBTYPE_RESULT);
    }
    else
    {
        if (lval_type == LOGF_SYMB_TYPE_OPRND_FLOAT)
        {
            if (lval_subtype == LOGF_SYMB_SUBTYPE_AN_INPUT)
            {
                pval = lval->pval;
                lvalf = ((class _logf_analog_input *)pval)->get_adcvalue_calibrated();
            }
            else //if (lval_subtype == LOGF_SYMB_SUBTYPE_RESULT) && ktes,volt,curr,temp
            {
                lvalf = lval->get_valf();
            }

        }
        else if (lval_type == LOGF_SYMB_TYPE_OPRND_BOOLEAN)
        {
            if (lval_subtype == LOGF_SYMB_SUBTYPE_DIG_INPUT)
            {
                pval = lval->pval;
                lvalf = (uint8_t)(  ((class _logf_digital_input *)pval)->get_pinvalue_withlogic()   );
            }
            else if (lval_subtype == LOGF_SYMB_SUBTYPE_RELAY)
            {
                pval = lval->pval;
                lvalf = (uint8_t)( ((class _logf_rele *)pval)->get_pinvalue()  );
            }
            else if (lval_subtype == LOGF_SYMB_SUBTYPE_FLAGS)
            {
                //lvalf =
            }
            else if (lval_subtype == LOGF_SYMB_SUBTYPE_RESULT)
            {
                lvalf = lval->get_valui();
            }
        }
        //
        if (rval_type == LOGF_SYMB_TYPE_OPRND_FLOAT)
        {
            if (rval_subtype == LOGF_SYMB_SUBTYPE_AN_INPUT)
            {
                pval = rval->pval;
                rvalf = ((class _logf_analog_input *)pval)->get_adcvalue_calibrated();
            }
            else //if (rval_subtype == LOGF_SYMB_SUBTYPE_RESULT) && ktes,volt,curr,temp
            {
                rvalf = rval->get_valf();
            }
        }
        else if (rval_type == LOGF_SYMB_TYPE_OPRND_BOOLEAN)
        {
            if (rval_subtype == LOGF_SYMB_SUBTYPE_DIG_INPUT)
            {
                pval = lval->pval;
                rvalf = (uint8_t)(  ((class _logf_digital_input *)pval)->get_pinvalue_withlogic()   );
            }
            else if (rval_subtype == LOGF_SYMB_SUBTYPE_RELAY)
            {
                pval = lval->pval;
                rvalf = (uint8_t)( ((class _logf_rele *)pval)->get_pinvalue()  );
            }
            else if (rval_subtype == LOGF_SYMB_SUBTYPE_FLAGS)
            {
                //rvalf =
            }
            else if (rval_subtype == LOGF_SYMB_SUBTYPE_RESULT)
            {
                rvalf = rval->get_valui();
            }
        }
        switch (op_subtype)
        {
            case LOGF_SYMB_SUBTYPE_RELATIONAL_OP:
                resultui = fx2OP_relational_op_float(lvalf, op_valui, rvalf );
                result.set_valui(resultui);
                result.set_type(LOGF_SYMB_TYPE_OPRND_BOOLEAN);
                //result.set_subtype(LOGF_SYMB_SUBTYPE_RESULT);
                break;

            case LOGF_SYMB_SUBTYPE_ARITHMETIC_OP:
                resultf = fx2OP_algebra_op_floats(lvalf, op_valui, rvalf);
                result.set_valf(resultf);
                result.set_type(LOGF_SYMB_TYPE_OPRND_FLOAT);
                //result.set_subtype(LOGF_SYMB_SUBTYPE_RESULT);
                break;

            default:
                break;
        }

    }
    result.set_subtype(LOGF_SYMB_SUBTYPE_RESULT);
    return result;
}


enum _cycle
{
    CYCLE_NONE,//close
    CYCLE_NOT,//CYCLE_1OP
    CYCLE_2OP,
};
enum
{
    STATE_1OP_NOT_NONE,
    STATE_1OP_NOT_OPERATOR,
    STATE_1OP_NOT_OPERAND,
};
enum
{
    STATE_2OP_NONE,
    STATE_2OP_LVAL,
    STATE_2OP_OPERATOR,
    STATE_2OP_RVAL,
};
enum
{
    RCSV_TYPE_NONE,
    RCSV_TYPE_1OP_NOT,
    RCSV_TYPE_PARENTHESIS,
};

class _logf_symbol _Symbol::logExp_eval_fr(const class _logf_symbol *logExp, uint8_t *counter_el, uint8_t rcsv_type)
{
    uint8_t cycle = CYCLE_NONE;
    uint8_t rcsv = 0;
    uint8_t counter_el_rcsv=0;
    uint8_t state_1OP = 0;
    uint8_t state_2OP = 0;
    //
    int8_t el_type;
    int8_t el_subtype;
    uint8_t el_valui;

    class _logf_symbol rval;
    class _logf_symbol op;
    class _logf_symbol _lval;
    class _logf_symbol lval;
    lval.set_type(LOGF_SYMB_TYPE_EOV);

    while (1)// (logExp->get_type() !=LOGF_SYMB_TYPE_EOF )
    {
        el_type = logExp->get_type();

        if (el_type == LOGF_SYMB_TYPE_EOV)
            break;

        //el = logExp_get_value(logExp);
        el_valui = logExp->get_valui();
        el_subtype = logExp->get_subtype();

        if ((el_subtype ==LOGF_SYMB_SUBTYPE_LOGICAL_OP) && (el_valui == LOGF_LOGICOP_NOT)) //(el == 'n')
        {
            if (cycle == CYCLE_NONE)
            {
                cycle = CYCLE_NOT;

                if (state_1OP == STATE_1OP_NOT_NONE)
                    state_1OP = STATE_1OP_NOT_OPERATOR;
            }
            else if (cycle == CYCLE_NOT)
            {
                rcsv = 1;
                lval = logExp_eval_fr(logExp, &counter_el_rcsv, RCSV_TYPE_1OP_NOT);

                if (state_1OP == STATE_1OP_NOT_OPERATOR)
                    state_1OP = STATE_1OP_NOT_OPERAND;
            }
            else if (cycle == CYCLE_2OP)
            {
                rcsv = 1;

                if (state_2OP == STATE_2OP_OPERATOR)
                {
                    rval = logExp_eval_fr(logExp, &counter_el_rcsv, RCSV_TYPE_1OP_NOT);
                    state_2OP = STATE_2OP_RVAL;
                }
            }
        }

        else if ((el_subtype==LOGF_SYMB_SUBTYPE_ASSOCIATIVITY_OP) && (el_valui == LOGF_ASSOC_OP_PARENTHESES_LEFT)) //(el == '(')
        {
            rcsv = 1;

            logExp++;
            (*counter_el)++;

            _lval = logExp_eval_fr(logExp, &counter_el_rcsv, RCSV_TYPE_PARENTHESIS);//regresa despues de haber encontrado ')'

            if (cycle == CYCLE_NONE)
            {
                lval = _lval;//simplemente un seguidor del ultimo lval
            }
            else if (cycle == CYCLE_NOT)
            {
                lval = _lval;

                if (state_1OP == STATE_1OP_NOT_OPERATOR)
                    state_1OP = STATE_1OP_NOT_OPERAND;
            }
            else if (cycle == CYCLE_2OP)
            {
                if (state_2OP == STATE_2OP_OPERATOR)
                {
                    rval = _lval;
                    state_2OP = STATE_2OP_RVAL;
                }
            }
        }
        else if ((el_subtype==LOGF_SYMB_SUBTYPE_ASSOCIATIVITY_OP) && (el_valui == LOGF_ASSOC_OP_PARENTHESES_RIGHT))//(el == ')')
        {
            (*counter_el)++;   //por su naturaleza, siempre regresa de una llamada recursiva
            break;          //originada por '(', osea, este *counter_el es el &counter_el_rcsv
            //pasado por la llamada anterior.
        }
        else if (//((el=='&') || (el=='|')) (el!= NOT)
            (el_type ==  LOGF_SYMB_TYPE_OPERATOR) &&
            !((el_subtype==LOGF_SYMB_SUBTYPE_LOGICAL_OP) && (el_valui==LOGF_LOGICOP_NOT))
        )
        {
            //op_type = el_subtype;
            if (cycle == CYCLE_2OP)
            {
                if (state_2OP == STATE_2OP_LVAL)
                {
                    //op = el;
                    op = *logExp;
                    state_2OP = STATE_2OP_OPERATOR;
                }
            }
            else if (cycle == CYCLE_NONE)
            {
                //op = el;
                op = *logExp;
                //x defecto sera tomado como una operacion de 2 op
                //por ejmp, puede provenir de una expr. iniciando con NOT,
                //en donde el LVAL es el resultado del NOT, a continuacion, pasa
                //a ser el LVAL & y se transforma en un ciclo de 2 op.
                cycle = CYCLE_2OP;
                state_2OP = STATE_2OP_OPERATOR;
            }
            //ojo: NOT es un cyclo que no incluye en su parsing "Operadores logicos"
        }
        else //solo operandos
            if ( (el_type == LOGF_SYMB_TYPE_OPRND_BOOLEAN) || (el_type == LOGF_SYMB_TYPE_OPRND_FLOAT)
                    //(el_subtype != LOGF_SYMB_TYPE_EOF) //)||
               )
            {
                if (cycle == CYCLE_NONE)
                {
                    cycle = CYCLE_2OP;

                    if (state_2OP == STATE_2OP_NONE)
                    {
                        //lval = el;
                        lval = *logExp;
                        state_2OP = STATE_2OP_LVAL;
                    }
                }
                else if (cycle == CYCLE_NOT)
                {
                    //lval = el;
                    lval = *logExp;

                    if (state_1OP == STATE_1OP_NOT_OPERATOR)
                        state_1OP = STATE_1OP_NOT_OPERAND;
                }
                else if (cycle == CYCLE_2OP)
                {
                    if (state_2OP == STATE_2OP_OPERATOR)
                    {
                        //rval = el;
                        rval = *logExp;
                        state_2OP = STATE_2OP_RVAL;
                    }
                }
            }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        if (rcsv == 0)
        {
            logExp++;            //incrementa normalmente
            (*counter_el)++;   //se suma el elemento actual(salvo '(' y ')' que tambien incrementan a *counter_el)
            //"en linea", puesto que no ha regresado en esta pasada de una recursion,
        }
        else//==1
        {
            logExp = logExp + counter_el_rcsv;
            *counter_el = *counter_el + counter_el_rcsv;
            //fix:
            counter_el_rcsv = 0;//x cada pasada debe de limpiarse estas 2 variables
            rcsv = 0;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        //Ejecucion de las funciones de 1 op:NOT y de 2 op: AND, OR, etc ///////
        //no se usa nada de punteros, ya fueron deferenciados a su correspondientes lval / op / rval
        if (cycle == CYCLE_NOT)
        {
            if (state_1OP == STATE_1OP_NOT_OPERAND)//NOT_TO_END)
            {
                //lval = ! (uint8_t)lval;
                uint8_t lvalui;
                void *pval;
                if (lval.get_subtype() == LOGF_SYMB_SUBTYPE_DIG_INPUT)
                {
                    pval = lval.pval;
                    lvalui = (uint8_t)( ((class _logf_digital_input *)pval)->get_pinvalue_withlogic() );
                }
                else if (lval.get_subtype() == LOGF_SYMB_SUBTYPE_RELAY)
                {
                    pval = lval.pval;
                    lvalui = (uint8_t)( ((class _logf_rele *)pval)->get_pinvalue() );
                }
                else if (lval.get_subtype() == LOGF_SYMB_SUBTYPE_FLAGS)
                {
                }
                else if (lval.get_subtype() == LOGF_SYMB_SUBTYPE_RESULT)
                {
                    lvalui = lval.get_valui();
                }
                //x defecto debe ser de tipo boolean, analizado por el parser
                lval.set_valui(!lvalui);
                lval.set_subtype(LOGF_SYMB_SUBTYPE_RESULT);

                cycle = CYCLE_NONE;
                state_1OP = STATE_1OP_NOT_NONE;
                if (rcsv_type == RCSV_TYPE_1OP_NOT)
                {
                    rcsv_type = RCSV_TYPE_NONE;
                    break;
                }
            }
        }
        else if (cycle == CYCLE_2OP)
        {
            if (state_2OP == STATE_2OP_RVAL)
            {
                //lval = (uint8_t)logical(lval, op_type, op, rval);
                lval = fx2OP(&lval, &op, &rval);

                cycle = CYCLE_NONE;
                state_2OP = STATE_2OP_NONE;
            }
        }

    }//end while

    //printf("lval es: %u\n", lval);
    return lval;

}

//uint8_t
class _logf_symbol _Symbol::logExp_eval(void)
{
    uint8_t counter_el=0;

    //class _logf_symbol result =  logExp_eval_fr(logExp_arr, &counter_el, RCSV_TYPE_NONE);
    class _logf_symbol result =  logExp_eval_fr(pdata->logExp_arr, &counter_el, RCSV_TYPE_NONE);
    //
    uint8_t subtype = result.get_subtype();
    uint8_t valui;
    float valf;
    void *pval;

    if (subtype != LOGF_SYMB_SUBTYPE_RESULT)//fue un seguidor de operando(float and bool) entonces.
    {
        pval = result.pval;

        switch (subtype)
        {
            case LOGF_SYMB_SUBTYPE_DIG_INPUT:
                valui = (uint8_t)(  ((class _logf_digital_input *)pval)->get_pinvalue_withlogic()   );
                result.set_valui(valui);
                break;

            case LOGF_SYMB_SUBTYPE_AN_INPUT:
                valf = ((class _logf_analog_input *)pval)->get_adcvalue_calibrated();
                result.set_valf(valf);
                break;

            case LOGF_SYMB_SUBTYPE_RELAY:
                valui = (uint8_t)( ((class _logf_rele *)pval)->get_pinvalue()  );
                result.set_valui(valui);
                break;

            //el resto de operandos mantiene su valor en valf y valui respectivamente
            //        case LOGF_SYMB_SUBTYPE_VOLTAGE:
            //            break;
            //        case LOGF_SYMB_SUBTYPE_CURRENT:
            //            break;
            //        case LOGF_SYMB_SUBTYPE_TEMPERATURE:
            //            break;
            //        case LOGF_SYMB_SUBTYPE_FLAGS:
            //            break;
            default:
                break;
        }
        subtype = LOGF_SYMB_SUBTYPE_RESULT;
    }
    return result;
}

void _Symbol::get_text(uint8_t el, char *text_out)
{
    //if (logf_symbol_data.logExp_arr[el].get_type() == LOGF_SYMB_TYPE_OPRND_BOOLEAN)
    uint8_t valui = pdata->logExp_arr[el].get_valui();
    float   valf  = pdata->logExp_arr[el].get_valf();

    switch (pdata->logExp_arr[el].get_subtype())
    {
        case LOGF_SYMB_SUBTYPE_DIG_INPUT:
            switch (valui)
            {
                case 0:
                    strcpy_P(text_out,  PSTR("D1"));
                    break;
                case 1:
                    strcpy_P(text_out,  PSTR("D2"));
                    break;
                case 2:
                    strcpy_P(text_out,  PSTR("D3"));
                    break;
                case 3:
                    strcpy_P(text_out,  PSTR("D4"));
                    break;
                default:
                    break;

            }
            break;

        case LOGF_SYMB_SUBTYPE_AN_INPUT:
            switch (valui)
            {
                case 0:
                    strcpy_P(text_out,  PSTR("A1"));
                    break;
                case 1:
                    strcpy_P(text_out,  PSTR("A2"));
                    break;
                case 2:
                    strcpy_P(text_out,  PSTR("A3"));
                    break;
                case 3:
                    strcpy_P(text_out,  PSTR("A4"));
                    break;
                default:
                    break;
            }
            break;

        case LOGF_SYMB_SUBTYPE_RELAY:
            switch (valui)
            {
                case 0:
                    strcpy_P(text_out,  PSTR("R1"));
                    break;
                case 1:
                    strcpy_P(text_out,  PSTR("R2"));
                    break;
                case 2:
                    strcpy_P(text_out,  PSTR("R3"));
                    break;
                case 3:
                    strcpy_P(text_out,  PSTR("R4"));
                    break;
                case 4:
                    strcpy_P(text_out,  PSTR("R5"));
                    break;
                default:
                    break;
            }
            break;
        //
        case LOGF_SYMB_SUBTYPE_LOGICAL_OP:
            switch (valui)
            {
                case LOGF_LOGICOP_AND:
                    strcpy_P(text_out,  PSTR("AND"));
                    break;
                case LOGF_LOGICOP_OR:
                    strcpy_P(text_out,  PSTR("OR"));
                    break;
                case LOGF_LOGICOP_NOT:
                    strcpy_P(text_out,  PSTR("NOT"));
                    break;
                default:
                    break;
            }
            break;

        case LOGF_SYMB_SUBTYPE_RELATIONAL_OP:
            switch (valui)
            {
                case LOGF_RELAT_OP_EQUAL_TO:
                    strcpy_P(text_out,  PSTR("=="));
                    break;
                case LOGF_RELAT_OP_NOTEQUAL_TO:
                    strcpy_P(text_out,  PSTR("!="));
                    break;
                case LOGF_RELAT_OP_LESSTHAN:
                    strcpy_P(text_out,  PSTR("<"));
                    break;
                case LOGF_RELAT_OP_GREATERTHAN:
                    strcpy_P(text_out,  PSTR(">"));
                    break;
                case LOGF_RELAT_OP_LESSTHAN_EQTO:
                    strcpy_P(text_out,  PSTR("<="));
                    break;
                case LOGF_RELAT_OP_GREATTHAN_EQTO:
                    strcpy_P(text_out,  PSTR(">="));
                    break;
                default:
                    break;
            }
            break;

        case LOGF_SYMB_SUBTYPE_ARITHMETIC_OP:
            switch (valui)
            {
                case LOGF_ARITH_OP_ADD:
                    strcpy_P(text_out,  PSTR("+"));
                    break;
                case LOGF_ARITH_OP_SUB:
                    strcpy_P(text_out,  PSTR("-"));
                    break;
                case LOGF_ARITH_OP_MUL:
                    strcpy_P(text_out,  PSTR("*"));
                    break;
                case LOGF_ARITH_OP_DIV:
                    strcpy_P(text_out,  PSTR("/"));
                    break;
                default:
                    break;
            }
            break;

        case LOGF_SYMB_SUBTYPE_ASSOCIATIVITY_OP:
            switch (valui)
            {
                case LOGF_ASSOC_OP_PARENTHESES_LEFT:
                    strcpy_P(text_out,  PSTR("("));
                    break;
                case LOGF_ASSOC_OP_PARENTHESES_RIGHT:
                    strcpy_P(text_out,  PSTR(")"));
                    break;
                case LOGF_ASSOC_OP_BRACKET_LEFT:
                    strcpy_P(text_out,  PSTR("["));
                    break;
                case LOGF_ASSOC_OP_BRACKET_RIGHT:
                    strcpy_P(text_out,  PSTR("]"));
                    break;
                case LOGF_ASSOC_OP_BRACE_LEFT:
                    strcpy_P(text_out,  PSTR("{"));
                    break;
                case LOGF_ASSOC_OP_BRACE_RIGHT:
                    strcpy_P(text_out,  PSTR("}"));
                    break;
                default:
                    break;
            }
            break;

        case LOGF_SYMB_SUBTYPE_PUNCTUATION_MARKS_OP:
            switch (valui)
            {
                case LOGF_PUNCT_OP_COMMA:
                    strcpy_P(text_out,  PSTR(","));
                    break;
                case LOGF_PUNCT_OP_SEMICOLON:
                    strcpy_P(text_out,  PSTR(";"));
                    break;
                case LOGF_PUNCT_OP_COLON:
                    strcpy_P(text_out,  PSTR(":"));
                    break;
                default:
                    break;
            }
            break;

        case LOGF_SYMB_SUBTYPE_ASSIGNMENT_OP:
            switch (valui)
            {
                case LOGF_ASSIGN_OP_EQUAL:
                    strcpy_P(text_out,  PSTR("="));
                    break;
                default:
                    break;
            }
            break;


        case LOGF_SYMB_SUBTYPE_LITERALS:

            dtostrf(valf,LITERAL_ASCHAR_BUFFWITDH, LITERAL_ASCHAR_NUMDECIMALS, text_out);

            break;

        case LOGF_SYMB_SUBTYPE_VOLTAGE:
            break;

        case LOGF_SYMB_SUBTYPE_CURRENT:
            break;

        case LOGF_SYMB_SUBTYPE_TEMPERATURE:
            break;

        case LOGF_SYMB_SUBTYPE_FLAGS:
            break;

        default:
            break;
    }
}

