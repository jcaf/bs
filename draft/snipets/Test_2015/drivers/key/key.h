#ifndef KEY_H_
#define KEY_H_

    #define KB_PIN_MENU 2
    #define KB_PIN_UP 21
    #define KB_PIN_DOWN 20
    #define KB_PIN_ENTER 19
    #define KB_PIN_ESCAPE 18
    //
    #define KB_NUM_KEYS 5
    //
    #define KB_KEY_CODE_MENU 1
    #define KB_KEY_CODE_UP 2
    #define KB_KEY_CODE_DOWN 3
    #define KB_KEY_CODE_ENTER 4
    #define KB_KEY_CODE_ESCAPE 5

    #define KB_KEY_CODE_RELEASE 0
    //
    #define KB_KEY_STATE_PRESSED 1
    #define KB_KEY_STATE_RELEASED 0
    //
    #define KB_COUNT_DEBOUNCE 4

    extern volatile uint8_t kb_keypressed;
    void kb_initialization(void);
    void kb_scan(void);
    void InitTCNT1(void);


#endif
