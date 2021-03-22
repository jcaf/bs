#ifndef PIZ_H_
#define PIZ_H_


//struct _piz
//{
////    struct _curpos
////    {
////        uint8_t num_el_inthisline;//
////        uint8_t vcol[DB_LOGF_NUM_SYMB_INLINE];//max numero de x linea
////    } curpos[DB_LOGF_LOGEXP_NUM_LINES_INBOARD];
//
////    struct _action
////    {
////        int8_t type;
////        int8_t subtype;
////    } action;
//
////    struct _data
////    {
////        int8_t newline;//new line
////        int8_t counter_el_inthisline;
////        int8_t colpos_inthisline;
////    } data;
//
////    struct _viewport
////    {
////        int8_t numline;
////    } viewport;
//};
//extern struct _piz piz;
//
//void piz_init(void);
//void piz_handler(void);
//void piz_setdata(struct _piz *piz);//set board's param
//void piz_viewport(struct _piz *piz);
//void piz_setcursor(uint8_t colpos_inthisline);

extern class _piz piz;
void piz_init(void);//x desaparecer
void piz_man(void);

#endif // PIZ_H_
