#include <LiquidCrystal.h>
#include "key.h"

#define LCD_ROW 4
#define LCD_COL 20
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);
#define LCD_PIN_BACKLIGHT 28

String pstr_vscroll[3];
uint8_t vscroll_posc_i;

void setup()
{
  pinMode(LCD_PIN_BACKLIGHT,OUTPUT);
  digitalWrite(LCD_PIN_BACKLIGHT,HIGH);
  
  lcd.begin(LCD_COL, LCD_ROW);
  lcd.setCursor(0,0);
  
  lcd.print("[MENU]");

  digitalWrite(13, LOW);
  pinMode(13,OUTPUT);

  kb_initialization();
  InitTCNT1();
  sei();

  //________________
  vscroll_posc_i=5;

  float b=1.248;
  String a=String(b);
  pstr_vscroll[0]=str_format(a,vscroll_posc_i);
  pstr_vscroll[1]=str_format("VC2=1.555V",vscroll_posc_i);
  pstr_vscroll[2]=str_format("VC3=3.458V",vscroll_posc_i);
  
  vscroll_add_mark(&pstr_vscroll[0]);
  
  lcd_show_disp();
}

#define VSCROLL_LIMIT_INF 0
#define VSCROLL_LIMIT_SUP 2

int8_t counter_UP=0;

void loop()
{
  if (kb_keypressed != KB_KEY_STATE_RELEASED)
  {
    switch (kb_keypressed)
    {
      case KB_KEY_CODE_MENU:

        digitalWrite(13, !digitalRead(13));
      break;

      case KB_KEY_CODE_UP:
        if (--counter_UP < VSCROLL_LIMIT_INF)
        {counter_UP = VSCROLL_LIMIT_INF;}

        vscroll_del_mark(&pstr_vscroll[counter_UP+1]);
        vscroll_add_mark(&pstr_vscroll[counter_UP]);
        lcd_show_disp();

        digitalWrite(13, !digitalRead(13));
      break;

      case KB_KEY_CODE_DOWN:
        
        if (++counter_UP > VSCROLL_LIMIT_SUP)
        {
          counter_UP = VSCROLL_LIMIT_SUP;
          
          //get more and show
          pstr_vscroll[0] = pstr_vscroll[1];
          pstr_vscroll[1] = pstr_vscroll[2];

          
          pstr_vscroll[2] = str_format("NUEVO",vscroll_posc_i);
          
          //

          //

        }
     
          vscroll_del_mark(&pstr_vscroll[counter_UP-1]);
          vscroll_add_mark(&pstr_vscroll[counter_UP]);
            
          lcd_show_disp();

        digitalWrite(13, !digitalRead(13));
      break;

      case KB_KEY_CODE_ENTER:
        digitalWrite(13, !digitalRead(13));
      break;

      case KB_KEY_CODE_ESCAPE:
        digitalWrite(13, !digitalRead(13));
      break;

    }

    kb_keypressed = KB_KEY_STATE_RELEASED;
  }

}

ISR(TIMER1_COMPB_vect)
{
    kb_scan();
}

void lcd_show_disp(void)
{
  uint8_t cr=0;//count row
  for (uint8_t r=1; r<LCD_ROW; r++)
  {
    lcd.setCursor(0,r);
    lcd.print(pstr_vscroll[cr]);
    cr++;
  }
}

void vscroll_add_mark(String * pstr)
{
  char arr[LCD_COL+1];
  pstr->toCharArray(arr,LCD_COL+1);
  arr[LCD_COL-1]='<';
  *pstr = arr;
}

void vscroll_del_mark(String * pstr)
{
  char arr[LCD_COL+1];
  pstr->toCharArray(arr,LCD_COL+1);
  arr[LCD_COL-1]=' ';
  *pstr = arr;
}


String str_format(String pstr,  uint8_t posc_i)
{
  char arr[LCD_COL+1];
  char arr1[LCD_COL+1];
  uint8_t length;
  uint8_t c=0;

  for (uint8_t i=0; i<LCD_COL; i++)
      {arr[i]=' ';}
 
  arr[LCD_COL]='\0';
  
  length = pstr.length();
  pstr.toCharArray(arr1, length+1);//analizar a fondo toCharArray

  
  uint8_t posc_f = posc_i+length;
  if (posc_f > LCD_COL)
    {posc_f=LCD_COL;}
  
  for (uint8_t i=posc_i; i<posc_f; i++)
  {
      arr[i]= arr1[c];
      c++;
  }
  return String(arr);
}
