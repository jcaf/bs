#ifndef VSCROLL_H_
#define VSCROLL_H_

#define _VSCROLL_DATATYPE_AS_INT8_T
//#define _VSCROLL_DATATYPE_AS_INT16_T

#ifdef _VSCROLL_DATATYPE_AS_INT8_T
typedef int8_t VSCROLLDT;//DataType
#endif // _VSCROLL_DATATYPE_AS_INT8_T

#ifdef _VSCROLL_DATATYPE_AS_INT16_T
typedef int16_t VSCROLLDT;//DataType
#endif // _VSCROLL_DATATYPE_AS_INT16_T


#define SIGN_CLEAR 0
#define SIGN_UP 1
#define SIGN_DOWN 2

#define FEAT_MARK_OFF 0
#define FEAT_MARK_ON 1


class VScroll
{
private:
    struct
    {
        String *data;
        VSCROLLDT dbsize;
        VSCROLLDT offset;
        VSCROLLDT base;
    } db;

    struct
    {
        String *data;
        VSCROLLDT  numline;
    } dispShowBuff;

    struct
    {
        struct
        {
            unsigned mark:1;
            unsigned __a :7;
        } bf;

    } feature;

    uint8_t signal;
    void add_mark(String *pstr);
    void del_mark(String *pstr);
    void clear_sign(void);
    void dispShowBuff_filldata(void);
    void process_signal(void);

    #define VSCROLL_STACK_DEEP 7
    struct _vscrollst
    {
        struct _stack
        {
            VSCROLLDT offset;
            VSCROLLDT base;
        } stack[VSCROLL_STACK_DEEP];

        VSCROLLDT sp;
    } vscrollst;

public:
    void init(String *db_data, VSCROLLDT db_dbsize, String *dispShowBuff_data, VSCROLLDT  dispShowBuff_numline,
              uint8_t mark,
              VSCROLLDT base=0,
              VSCROLLDT offset=0
             );
    //uint16_t
    VSCROLLDT get_markPosc(void);
    void sign_up(void);
    void sign_down(void);
    void job(void);
    void feat_mark_on(void);
    void feat_mark_off(void);
    //void del_mark_atPosc(void);
    void set_db(String *db_data);
    //
    VSCROLLDT offset;
    VSCROLLDT base;
    void stack_push(void);
    void stack_pop(uint8_t n=1);//1 pop x default
    void base_offset_reset(void);
};

extern VScroll vscroll;

#define DISP_SHOW_BUFF_LENGTH 3
extern String dispShowBuff[DISP_SHOW_BUFF_LENGTH];

#endif





