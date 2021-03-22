#include <Arduino.h>
#include "key.h"

volatile uint8_t kb_keypressed = KB_KEY_STATE_RELEASED;

uint8_t kb_read_pin_menu(void) {return digitalRead(KB_PIN_MENU);}
uint8_t kb_read_pin_up(void) {return digitalRead(KB_PIN_UP);}
uint8_t kb_read_pin_down(void) {return digitalRead(KB_PIN_DOWN);}
uint8_t kb_read_pin_enter(void) {return digitalRead(KB_PIN_ENTER);}
uint8_t kb_read_pin_escape(void) {return digitalRead(KB_PIN_ESCAPE);}

//typedef void (*PTRFXrVOID)(void);
typedef uint8_t (*PTRFXrUINT8_T)(void);


  struct _kb
  {
    uint8_t sm0;
    uint8_t counter0;
    PTRFXrUINT8_T kb_read;
  }kb[KB_NUM_KEYS];


  void kb_initialization(void)
  {
    pinMode(KB_PIN_MENU, INPUT);
    pinMode(KB_PIN_UP, INPUT);
    pinMode(KB_PIN_DOWN, INPUT);
    pinMode(KB_PIN_ENTER, INPUT);
    pinMode(KB_PIN_ESCAPE, INPUT);

    kb[0].kb_read = &kb_read_pin_menu;
    kb[1].kb_read = &kb_read_pin_up;
    kb[2].kb_read = &kb_read_pin_down;
    kb[3].kb_read = &kb_read_pin_enter;
    kb[4].kb_read = &kb_read_pin_escape;
  }


  void kb_scan(void)
  {
    uint8_t k;

    for (k=0; k<KB_NUM_KEYS; k++)
    {
        if (kb[k].sm0 == 0)
        {
          if (kb[k].kb_read() == KB_KEY_STATE_PRESSED)
            kb[k].sm0++;

        }
        else if (kb[k].sm0 == 1)
        {
          if (++kb[k].counter0 == KB_COUNT_DEBOUNCE)
          {
            kb[k].counter0 = 0x00;

            if (kb[k].kb_read() != KB_KEY_STATE_PRESSED)
            {
              kb[k].sm0--;
            }
            else
            {
              switch (k)
              {
                case 0:kb_keypressed = KB_KEY_CODE_MENU;break;
                case 1:kb_keypressed = KB_KEY_CODE_UP;break;
                case 2:kb_keypressed = KB_KEY_CODE_DOWN;break;
                case 3:kb_keypressed = KB_KEY_CODE_ENTER;break;
                case 4:kb_keypressed = KB_KEY_CODE_ESCAPE;break;
              }

              kb[k].sm0++;
            }
          }
        }
        else if (kb[k].sm0 == 2)
        {
           if (kb[k].kb_read() == KB_KEY_STATE_RELEASED)
            kb[k].sm0++;

        }
        else if (kb[k].sm0 == 3)
        {
          if (++kb[k].counter0 == KB_COUNT_DEBOUNCE)
          {
            kb[k].counter0 = 0x00;

            if (kb[k].kb_read() != KB_KEY_STATE_RELEASED)
            {
              kb[k].sm0--;
            }
            else
            {
              kb[k].sm0 = 0x00;
            }
          }
        }
    }
  }


#define FREQ_SYSTEM 16000000.0
#define ms 0.001
#define KTE_TIME_FOR_CTC(TimeForCTC, PRESCALER_TCNT0_XXXX ) ((byte)( ((TimeForCTC)*FREQ_SYSTEM)/ PRESCALER_TCNT0_XXXX ) )
void InitTCNT1(void)
{
 // noInterrupts();
  TCCR1B = (1 << WGM12)|(1 << CS12) |(0 << CS11)|(1 << CS10);//CTC PRES=1024
  OCR1A = KTE_TIME_FOR_CTC(5*ms, 1024.0 );
  TIMSK1 |= 1<<OCIE1B;
  //interrupts();
}
