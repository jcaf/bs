#ifndef SYSTEM_H_
#define SYSTEM_H

    //#include <LiquidCrystal.h>
    #include "lcd_fxaux.h"
    #include "vscroll.h"

    #define DEBUG_SYSTEM

    #define WIZNET_CS 10
    #define SDCARD_CS 4
    //#define ATMEGA_SPI_SS 53

    typedef uint8_t (*PTRFX_retUINT8_T)(void);
    typedef uint16_t (*PTRFX_retUINT16_T)(void);
    typedef void (*PTRFX_retVOID)(void);


    #define DTOSTRF_PREC 3
    #define DTOSTRF_NUM_INT 2

    #define DTOSTRF_WIDTH (1+DTOSTRF_NUM_INT+1+DTOSTRF_PREC) //SIGN+NUM INT+.+DECIMALS
    #define DTOSTRF_SIZE_BUF (DTOSTRF_WIDTH+1)               //DTOSTRF_WIDTH+ NULL_TERMINATOR


    //extern LiquidCrystal lcd;
    extern mylcd lcd;
    extern VScroll vscroll;



    #define DISP_SHOW_BUFF_LENGTH 3
    extern String dispShowBuff[DISP_SHOW_BUFF_LENGTH];

//    struct _scheduler
//    {
//      struct
//      {
//        struct
//        {
//          unsigned ltc6804:1;
//          unsigned __a:7;
//        }bf;
//
//      }job;
//    };

    //extern struct _scheduler sch;
    void main_menu_go(void);

    struct _scheduler
    {
        uint8_t sm0;
        uint8_t sm1;
    };

    extern struct _scheduler scheduler;

    #define FUNCT_MODE_PRODUCTION   0
    #define FUNCT_MODE_DEBUG        1
    #define FUNCT_MODE_TESTING      2

    struct _system
    {
        uint8_t funct_mode;
    };
    extern struct _system system;


    void sysPrint_serialPrint(String msg);
    void sysPrint_serialPrintln(String msg);

    void sysPrint_lcdPrint(uint8_t fil, int8_t col, String msg);

    void sysPrint_lcdPrintPSTR(uint8_t fil, int8_t col, const char *pmsg);

    void sysPrint_lcd_serialPrintln(uint8_t fil, int8_t col, String msg);


    ////////////////////////////////////////////////////////////////////
    #define MHz (1000000)
	#define KHz (1000.0)
	#define ms	(0.001)
	#define us	(0.000001)

    //#define FREQ_SYSTEM (8000000)
	#define FREQ_SYSTEM (16000000)


//#define _delay_ms(x) DelayMs( (unsigned char) x)
//#define _delay_us(x) DelayUs( (unsigned char) x)


    #if defined __ICCAVR__
    //#ifndef __GNUC__
        #define PSTR(s) (s)
        #define sei() do{ __enable_interrupt();}while(0)
        #define cli() do{ __disable_interrupt();}while(0)
        //
        #define PRAGMA(text) _Pragma( #text )
        #define ISR(VECT) PRAGMA(vector=VECT) __interrupt void VECT##_fxInt(void)
     #endif

    #if defined(__ICCAVR__) // IAR C Compiler
        #define FLASH_DECLARE(x) __flash x
        //
        #define FoR_DECLARE(x) FLASH_DECLARE(x)     //Flash or Ram: Si estar?? en FLASH
        //#define FoR_DECLARE(x) x                  //Si estar?? en RAM
    #endif

    #if defined(__GNUC__) // GNU Compiler
        #define FLASH_DECLARE(x) x __attribute__((__progmem__))
        //
        #define FoR_DECLARE(x) FLASH_DECLARE(x)     //Flash or Ram: Si estar?? en FLASH
        //#define FoR_DECLARE(x) x                  //Si estar?? en RAM
    #endif

    #if defined __GNUC__
        #define NOP() do{asm volatile ("nop");}while(0)
    #elif defined __ICCAVR__
        #define NOP() do{__no_operation();}while(0)
    //#elif
    //#elif
    #endif


	#define io7 (1<<7)
	#define io6 (1<<6)
	#define io5 (1<<5)
	#define io4 (1<<4)
	#define io3 (1<<3)
	#define io2 (1<<2)
	#define io1 (1<<1)
	#define io0 (1<<0)

	#define SettingBitsInRegister(REGISTER,b7,b6,b5,b4,b3,b2,b1,b0) do{REGISTER=((b7<<7)|(b6<<6)|(b5<<5)|(b4<<4)|(b3<<3)|(b2<<2)|(b1<<1)|(b0<<0));}while(0)


	#define _OrRegister(_REGISTER_, _KTE_) 	do{_REGISTER_ |= (_KTE_); }while(0)
	#define _AndRegister(_REGISTER_,_KTE_) 	do{_REGISTER_ &= (_KTE_); }while(0)
//
	#define BitTo1(_REGISTER_, _KTE_)	_OrRegister(_REGISTER_, (1<<_KTE_))
	#define BitTo0(_REGISTER_, _KTE_)	_AndRegister(_REGISTER_, (~(1<<_KTE_)))


	#define TogglingBit(_REGISTER_,_BIT_) do{_REGISTER_ ^= (1<<_BIT_);}while(0)
	//

    #if defined __PICC18__ || defined __PICC__  //etc
   		#define INPUT_PIN 	1	//MICROCHIP
		#define OUTPUT_PIN 	0	//

		#define ConfigInputPin(IOREGISTER,_PIN_) 	do{ IOREGISTER |= (1<<_PIN_);  }while(0)
		#define ConfigOutputPin(IOREGISTER,_PIN_) do{ IOREGISTER &= (~(1<<_PIN_)); }while(0)

		//#define ConfigInputPort(IOREGISTER, BITS_INPUT) do(IOREGISTER |=  (BITS_INPUT); )while(0)	//pic
		#define ConfigInputPort(IOREGISTER,  BITS_INPUT) _OrRegister(IOREGISTER, BITS_INPUT)
		#define ConfigOutputPort(IOREGISTER, BITS_INPUT) _AndRegister(IOREGISTER, (~(BITS_INPUT)) )

    #else   //__GNUC__ __ICCAVR__
        #define INPUT_PIN 	0	//
        #define OUTPUT_PIN 	1	//

    	#define ConfigInputPin(IOREGISTER,_PIN_) do{IOREGISTER &= (~(0x01<<_PIN_));}while(0)
    	#define ConfigOutputPin(IOREGISTER,_PIN_) do{IOREGISTER |= (0x01<<_PIN_);}while(0)

    	//#define ConfigInputPort(IOREGISTER, BITS) do(IOREGISTER &=  (~(BITS)); )while(0)	//avr
    	#define ConfigInputPort(IOREGISTER,  BITS_INPUT) _AndRegister(IOREGISTER, (~(BITS_INPUT)) )
		#define ConfigOutputPort(IOREGISTER, BITS_INPUT) _OrRegister(IOREGISTER, BITS_INPUT)


    #endif


    //Escritura de Pin
    #define PinTo0(PORTX,_PIN_) do{PORTX &= (~(0x01<<_PIN_));}while(0)
    #define PinTo1(PORTX,_PIN_) do{PORTX |= (0x01<<_PIN_);}while(0)

    //Lectura de Pin
    #define ReadPin(PINX,_PIN_) ((PINX & (1<<_PIN_))>>_PIN_)

	#define TRUE 		(1)
	#define FALSE 		(0)

    #define OFF         (0)
    #define ON          (1)

	#define ENABLE  	(1)
	#define DISABLE 	(0)

	#define CLEAR 		(0)	//Universal en cualquier microcontroller,,, NO CAMBIAR.. Si sucediera el caso.
	#define SET			(1)	//verificar los perifericos que hacen uso de estas definiciones...

	#define CLEAR_PORT	(0x00)
	#define SET_PORT	(0xFF)

	//Explicito en paso de parametros a funcion
	#define _PARAMIn
	#define _PARAMOut

	/**************************************************************************************************

	**************************************************************************************************/
	#define _BYTE_WIDTH_	(8)	//Ancho de 1 byte en bits
	#define _WORD_WIDTH_	(16)//Ancho de 1 word en bits

	/**************************************************************************************************

	**************************************************************************************************/
	typedef unsigned char BOOL;
	/////////////////////////////////////////////////////////////////////////////////////


#endif
