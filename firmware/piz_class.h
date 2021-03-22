#ifndef PIZ_CLASS_H_
#define PIZ_CLASS_H_

#include "logf_class.h"         //LOGEXP_SYMBOL_NUM_ELEMENTS

enum _PIZ_ACTION_TYPE
{
    PIZ_IDLE_ACTION = 0,
    PIZ_SYMBOL_ACTION,
    PIZ_CURSOR_ACTION,
};

enum _PIZ_ACTION_SUBTYPE
{
    PIZ_SYMBOL_ASSIGN_ACTION_NONE = 0,
    PIZ_SYMBOL_ASSIGN_ACTION_NEW,
    PIZ_SYMBOL_ASSIGN_ACTION_REPLACE,
    PIZ_SYMBOL_ASSIGN_ACTION_INSERT,
    PIZ_SYMBOL_ASSIGN_ACTION_DELETE,
    PIZ_SYMBOL_ASSIGN_ACTION_DELETEALL,
    //
    PIZ_CURSOR_ACTION_LEFT,
    PIZ_CURSOR_ACTION_RIGHT,
};
////////////////////////////////////////////
enum _ELEMENT_TYPE
{
    EOV = 0,
    LOGF_SYMB,
    ASSIGNMENT,
    POINTER_TO_NEXT,
};
enum _ELEMENT_SUBTYPE
{
//    ELEMENT_SUBTYPE_,
//    ELEMENT_SUBTYPE_,
};

struct _piz_el_coord
{
    int8_t f;//fila
    int8_t c;//col
};
typedef struct _piz_el_coord PIZ_EL_COORD;

class _piz_el
{
private:
    PIZ_EL_COORD coord;
    int8_t type;
    int8_t subtype;

public:
    void set_coord(PIZ_EL_COORD coord);
    PIZ_EL_COORD get_coord(void);

    void set_coord_f(int8_t);
    void set_coord_c(int8_t);

    int8_t get_coord_f(void);
    int8_t get_coord_c(void);

    void set_type(int8_t type);
    void set_subtype(int8_t subtype);
    int8_t get_type(void);
    int8_t get_subtype(void);
};

class _el_saved
{
public:
    class _piz_el el;
    int8_t ptr;
};

class _piz_el_mgt
{
    int8_t ptr;

    class _el_saved el_saved;
public:
    class _piz_el el_arr[LOGEXP_SYMBOL_NUM_ELEMENTS];//logf_symbol + assignment

    void add(const class _piz_el &el);
    int8_t getptr(void);
    void resetptr(void);
    void setptr(int8_t ptr);
    void incptr(void);
    void decptr(void);
    uint8_t get_length(void);

    int8_t get_type(void);
    int8_t get_subtype(void);

    PIZ_EL_COORD get_coord(void);

    void save(int8_t ptr);
    class _el_saved get_saved(void);


    class _piz_el get_pointed_element(void);
    class _piz_el get_element(int8_t i);
};

///////////////////////////////////////////
struct _piz_viewport_config
{
    int8_t base;//tipos de datos de vscroll
    int8_t offset;
};
typedef struct _piz_viewport_config PIZ_VP_CONFIG;

class _piz_viewport
{
private:
    PIZ_VP_CONFIG coord;
public:
    //
    VScroll *pvscroll;
    void set_vscroll(VScroll *pvscroll);
    //
    void set_lastconfig(PIZ_VP_CONFIG coord);
    PIZ_VP_CONFIG get_lastconfig(void);

    int8_t numline;

    void set_numline(int8_t nl);
    int8_t get_numline(void);
};

///////////////////////////////////////////
#define PIZ_BLANKSPACE_NUMCHARS 1


class _piz_cursor
{
private:
    int8_t f;
    int8_t c;
public:

    void set_f(int8_t f)
    {
        this->f = f;
    }
    void set_c(int8_t c)
    {
        this->c = c;
    }
    int8_t get_f(void)
    {
        return f;
    }
    int8_t get_c(void)
    {
        return c;
    }

    void blinkat_fc(void)
    {
        lcd.setCursor(c,f);
        //lcd.blink();
    }
};

struct _builder_state
{
    struct _cursor
    {
        int8_t nextpos;
    } cursor;
};

struct _vp_status
{
    VSCROLLDT db_offset;
    VSCROLLDT db_base;
    int8_t numline;
};

class _piz
{

public:
    struct _prop
    {
        int8_t margen_right;//manejarlo a traves del constructor
        int8_t numlines;

        struct _line
        {
            int8_t num_el;
        } line[DB_LOGF_LOGEXP_NUM_LINES_INBOARD];

        struct _inthisline
        {
            int8_t counter_el;
        } inthisline;

    } prop;

    struct _action
    {
        int8_t type;
        int8_t subtype;
    } action;


    class _piz_el_mgt el;

    class _piz_viewport vp;
    //
    char (*pdbstring)[LCDAN_STR_MAX_SIZE];
    void builder_content_set_dbstring(char (*pdbstring)[LCDAN_STR_MAX_SIZE]);
    //void
    struct _builder_state builder_content(void);
    //
    void viewport(void);
    class _piz_cursor cursor;

    void management(void);

    _piz(int8_t vp_numline=1, int8_t action_type=PIZ_IDLE_ACTION)//constructor
    {
      vp.numline = vp_numline ;
      action.type = action_type;
      //
      el.el_arr[0].set_type(_ELEMENT_TYPE::POINTER_TO_NEXT);
    }
};

//////////////////////////////////////////////
class _piz_build_dbstring //local
{
public:
    int8_t numline;

    char (*pdbstring)[LCDAN_STR_MAX_SIZE];

    _piz_build_dbstring(char (*pdbstring)[LCDAN_STR_MAX_SIZE])//constructor
    {
        this->pdbstring = pdbstring;
    }

    void set_db(char (*pdbstring)[LCDAN_STR_MAX_SIZE])
    {
        this->pdbstring = pdbstring;
    }

    void set_numline(int8_t nl);
    int8_t get_numline(void);

    char text_out[LOGF_SYMBOL_TEXT_MAX_SIZE];
    char str[LCDAN_STR_MAX_SIZE];
    struct _bf
    {
        unsigned add_blankspace: 1;
        unsigned __a:7;
    } bf;
    void set_addBlankspace(uint8_t bs);
    void build_line_content(void);

    void add_row(int8_t line, char *str);

    void str_reset(void);
};

#endif // PIZ_CLASS_H_
