#ifndef LOGF_CLASS_H_INCLUDED
#define LOGF_CLASS_H_INCLUDED

#include "digital_input.h"
#include "analog_input.h"
#include "rele.h"
#include "types.h"

enum
{
    LOGF_DIGINPUT_NEGATIVE_LOGIC,
    LOGF_DIGINPUT_POSITIVE_LOGIC,
};
class _logf_digital_input
{
    uint8_t logic;
    PTRFX_retUINT8_T fx_read_pin;
public:
    void set_addr_fx_read_pin(PTRFX_retUINT8_T addr_read_pin);
    void set_logic(uint8_t l);
    uint8_t get_logic(void);
    uint8_t get_pinvalue_internal(void);
    uint8_t get_pinvalue_withlogic(void);
};

enum
{
    LOGF_ANINPUT_UNITS_ADIMENSIONAL,
};
class _logf_analog_input
{
//    double scale_factor;
//    double offset;
//    uint8_t units;
//
    PTRFX_retUINT16_T fx_read_pin;
public:
    struct _an_input_factors an_input_factors;

    void set_addr_fx_read_pin(PTRFX_retUINT16_T addr_fx_read_pin);
    void set_scale_factor(double sf);
    void set_offset(double o);
    void set_units(uint8_t u);
    double get_scale_factor(void);
    double get_offset(void);
    uint8_t get_units(void);
    uint16_t get_adcvalue_internal(void);
    double get_adcvalue_calibrated(void);
};

struct _logf_rele_bf
{
    unsigned enable:1;
    unsigned busy:1;
    unsigned __a:6;
};
extern struct _logf_rele_bf logf_rele_bf[NUM_RELE_MAX];

class _logf_rele
{
    PTRFX_retVOID_arg1_UINT8_T fx_write_pin;
    PTRFX_retUINT8_T fx_read_pin;
public:
//    struct _bf
//    {
//        unsigned enable:1;
//        unsigned busy:1;
//        unsigned __a:6;
//    } bf;
    struct _logf_rele_bf bf;

    void set_addr_fx_write_pin(PTRFX_retVOID_arg1_UINT8_T addr_fx_write_pin);
    void set_value(uint8_t value);
    void set_addr_fx_read_pin(PTRFX_retUINT8_T addr_fx_read_pin);
    uint8_t get_pinvalue(void);

};
////////////////////////////////////////////////////////
enum _LOGF_SYMB_TYPE
{
    LOGF_SYMB_TYPE_EOV,//end of expression
    //
    LOGF_SYMB_TYPE_OPRND_BOOLEAN,//operand
    LOGF_SYMB_TYPE_OPRND_FLOAT,//operand
    LOGF_SYMB_TYPE_OPERATOR,//operator
};
enum _LOGF_SYMB_SUBTYPE
{
    LOGF_SYMB_SUBTYPE_DIG_INPUT,
    LOGF_SYMB_SUBTYPE_AN_INPUT,
    LOGF_SYMB_SUBTYPE_RELAY,
    //
    LOGF_SYMB_SUBTYPE_LOGICAL_OP,
    LOGF_SYMB_SUBTYPE_RELATIONAL_OP,
    LOGF_SYMB_SUBTYPE_ARITHMETIC_OP,
    LOGF_SYMB_SUBTYPE_ASSOCIATIVITY_OP,
    LOGF_SYMB_SUBTYPE_PUNCTUATION_MARKS_OP,
    LOGF_SYMB_SUBTYPE_ASSIGNMENT_OP,
    LOGF_SYMB_SUBTYPE_LITERALS,
    //
    LOGF_SYMB_SUBTYPE_VOLTAGE,
    LOGF_SYMB_SUBTYPE_CURRENT,
    LOGF_SYMB_SUBTYPE_TEMPERATURE,
    LOGF_SYMB_SUBTYPE_FLAGS,
    //
    LOGF_SYMB_SUBTYPE_RESULT,//result of expression
};

enum _LOGICAL_OP
{
    LOGF_LOGICOP_AND,
    LOGF_LOGICOP_OR,
    LOGF_LOGICOP_NOT,
};

enum _RELATIONAL_OP
{
    LOGF_RELAT_OP_EQUAL_TO,
    LOGF_RELAT_OP_NOTEQUAL_TO,
    LOGF_RELAT_OP_LESSTHAN,
    LOGF_RELAT_OP_GREATERTHAN,
    LOGF_RELAT_OP_LESSTHAN_EQTO,
    LOGF_RELAT_OP_GREATTHAN_EQTO,
};
enum _ARITHMETIC_OP
{
    LOGF_ARITH_OP_ADD,
    LOGF_ARITH_OP_SUB,
    LOGF_ARITH_OP_MUL,
    LOGF_ARITH_OP_DIV,
};

enum _ASSOCIATIVITY_OP  //bracket logf_symbols
{
    LOGF_ASSOC_OP_PARENTHESES_LEFT,//(
    LOGF_ASSOC_OP_PARENTHESES_RIGHT,//)
    LOGF_ASSOC_OP_BRACKET_LEFT,//[
    LOGF_ASSOC_OP_BRACKET_RIGHT,//]
    LOGF_ASSOC_OP_BRACE_LEFT,//{
    LOGF_ASSOC_OP_BRACE_RIGHT,//}
};

enum _PUNCTUATION_MARKS_OP
{
    LOGF_PUNCT_OP_COMMA,// ,
    LOGF_PUNCT_OP_SEMICOLON,// ;
    LOGF_PUNCT_OP_COLON,// :
};

enum _ASSIGNMENT_OP
{
    LOGF_ASSIGN_OP_EQUAL,// =
};


class _logf_symbol
{
    int8_t type;
    int8_t subtype;
    uint8_t valui;  //bool valui
    float valf;     //by val
public:
    _logf_symbol(int8_t type = LOGF_SYMB_TYPE_EOV,//0
                 int8_t subtype = 0,
                 uint8_t valui = 0,
                 float valf = 0.0f
                )// (void *) 0x00 falta el puntero pval
    {
        this->type = type;
        this->subtype = subtype;
        this->valui = valui;
        this->valf = valf;
    }
    void set_valui(uint8_t val);
    uint8_t get_valui(void) const;

    void set_valf(float val);
    float get_valf(void) const;

    void *pval;//by ref
    //
    void set_type(int8_t subtyp);
    int8_t get_type(void) const;

    void set_subtype(int8_t subtyp);
    int8_t get_subtype(void) const;
};
class _Symbol
{
public:
    struct _logf_logExp_assigOut_data *pdata;
    void set_data_addr(struct _logf_logExp_assigOut_data *_pdata);

    //logExp
    uint8_t add(const class _logf_symbol &symbol );
    uint8_t replace(const class _logf_symbol &symbol );
    uint8_t insert(const class _logf_symbol &symbol );
    void    clear(void);
    void    incptr(void);
    void    decptr(void);
    int8_t  getptr(void);
    void    setptr(int8_t ptr);
    void    get_text(uint8_t el, char *text_out);
    uint8_t get_length(void);

    class _logf_symbol logExp_eval(void);
    class _logf_symbol fx2OP(const class _logf_symbol *lval, const class _logf_symbol *op, const class _logf_symbol *rval);
    class _logf_symbol logExp_eval_fr(const class _logf_symbol *logExp, uint8_t *counter_el, uint8_t rcsv_type);

    uint8_t fx2OP_logical_op_boolean(uint8_t lval, uint8_t op, uint8_t rval);
    uint8_t fx2OP_relational_op_boolean(uint8_t lval, uint8_t op, uint8_t rval);
    uint8_t fx2OP_algebra_op_boolean(uint8_t lval, uint8_t op, uint8_t rval);
    uint8_t fx2OP_relational_op_float(float lval, uint8_t op, float rval);
    float   fx2OP_algebra_op_floats(float lval, uint8_t op, float rval);
};

enum LOGF_ASSIG_TYPE
{
    LOGF_ASSIG_TYPE_EOV,
    LOGF_ASSIG_TYPE_DIRECT_RELAY,
    LOGF_ASSIG_TYPE_FX,
};
enum LOGF_ASSIG_SUBTYPE
{
    LOGF_ASSIG_SUBTYPE_FX_TIMER,
};

class _logf_output//data
{
    int8_t type;
    int8_t subtype;

    uint8_t rele;
    uint8_t rele_activeif;
    float valf;
public:
    _logf_output(
        int8_t type = LOGF_ASSIG_TYPE_EOV,//0
        int8_t subtype = 0,
        uint8_t rele = 0,
        uint8_t rele_activeif = 0,
        float valf = 0.0f
    )
    {
        this->type = type;
        this->subtype = subtype;
        this->rele = rele;
        this->rele_activeif = rele_activeif;
        this->valf = valf;
    }

    void set_type(int8_t typ);
    uint8_t get_type(void) const;

    void set_subtype(int8_t subtyp);
    uint8_t get_subtype(void) const;

    void set_rele(uint8_t r);
    void set_rele_activeif(uint8_t ai);
    uint8_t get_rele(void);
    uint8_t get_rele_activeif(void);

    void set_valf(float _valf);
    float get_valf(void);
};

class _logf_outAssign//fx over data
{
public:
    struct _logf_logExp_assigOut_data *pdata;
    void set_data_addr(struct _logf_logExp_assigOut_data *_pdata);

    uint8_t add(const class _logf_output &logf_output);
    uint8_t replace(const class _logf_output &logf_output);
    uint8_t insert(const class _logf_output &logf_output);

    void clear(void);

    void    incptr(void);
    void    decptr(void);
    int8_t getptr(void);
    void    setptr(int8_t ptr);

    uint8_t get_length(void);
    uint8_t get_array_rele_available(uint8_t *rele_available);
    uint8_t get_num_rele_available(void);

    void get_text(uint8_t el, char *text_out);
};


//definiciones para la base de datos creado en menu_logf_newFx.cpp
#define DB_LOGF_LOGEXP_NUM_LINES_INBOARD 6//10 //Max. Numero de lineas creadas en el LCD
#define DB_LOGF_EXP_TEXT_OUT_MIN_LENGTH 2//2 caracters q representen algun operando

#define DB_LOGF_NUM_SYMB_INLINE ((LCD_COL-2)/DB_LOGF_EXP_TEXT_OUT_MIN_LENGTH)
//
#include "lcd_fxaux.h"
//se suman en ambos una posicion mas para el EOF
#define LOGEXP_SYMBOL_NUM_ELEMENTS ((DB_LOGF_LOGEXP_NUM_LINES_INBOARD * DB_LOGF_NUM_SYMB_INLINE) +1) //+1(EOF)
#define LOGEXP_OUTPUT_NUM_ELEMENTS (NUM_RELE_MAX+1)//+EOV

struct _logf_logExp_assigOut_data
{
    class _logf_symbol logExp_arr[LOGEXP_SYMBOL_NUM_ELEMENTS];
    int8_t logExp_ptr;//element-->//deberia guardar su contador-> OK
    uint16_t logExp_length;

    class _logf_output outAssign_arr[LOGEXP_OUTPUT_NUM_ELEMENTS];
    int8_t outAssign_ptr;//idx for output[LOGEXP_OUTPUT_NUM_ELEMENTS]
};

#define LOGF_SYMBOL_TEXT_MAX_SIZE 10

//setnum definitions for litelal
#define LITERAL_SETNUM_CHARNUMBER_SELECTED 0 // the name/orden of the number NAME_setnum
#define LITERAL_ASCHAR_NUMINTEGERS 5
#define LITERAL_ASCHAR_NUMDECIMALS 2
// automatic
#define LITERAL_ASCHAR_BUFFWITDH (LITERAL_ASCHAR_NUMINTEGERS + 1 + LITERAL_ASCHAR_NUMDECIMALS)
#define LITERAL_ASCHAR_DIGPOS_DP LITERAL_ASCHAR_NUMINTEGERS
#define LITERAL_ASCHAR_DIGPOS_ENDOFDIGIT (LITERAL_ASCHAR_NUMINTEGERS + 1 + LITERAL_ASCHAR_NUMDECIMALS) - 1
//

#endif // LOGF_CLASS_H_INCLUDED
