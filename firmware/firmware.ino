/*
22/03/2021
Firmare:
----------
- Nueva estructura de carpetas, "firmware" es la carpeta contenedora. (draft es el histórico)
- El renombra el proyecto a firmware.ino para Arduino, y firmware.cbp para CodeBlocks

BUG FIXED
1) La presentación de la temperatura en el LCD causaba un reset porque no habia suf. espacio en dtostr() para el charbuff[] de salida. Fixed
2) La temperatura estaba muy cerca entre las 2 ktes de "fuera de rango y no conectada", al ser float se compara entre un rango dentro del punto medio
3) La corriente después de serobtenida era truncada a 0, sumado con el Factor de corrección al final.. ahora se contempla antes el F.C
4) La temperatura desde el registro del LTC ahora tiene un umbral menor para que sea considerado como no conectado de ser el caso. Si el máximo
 es 30000, se usa 29700, q corresponde a 2.970V en VREF2
5) Las ktes de temperatura ahora son

#define NTC10K_NO_CONNECTED -600
#define NTC10K_OUT_RANGE -700

6) La lectura del año es corregido para mostrar el digito de mayor peso
 //Year 00..99
    Y10 = (I2Ccfx_UserBuff[6]>>4);// & 0x01; Bug fixed
    Y = I2Ccfx_UserBuff[6] & 0x0F;

- Por mejorar:
Consistencias dividendo y divisor en la funcion de obtener Temperatura. [OK, realizado]
*Pasar las ktes OUT OF RANGE y NO CONNECTED a enteros y la temperatura a float... por definir

Hardware:
1) Se reemplazó el LTC6804 del stack#2 del Master
2) Todos los mini-cktos switching deL LTC3900 estan OK
3) La impedancia del stack#5 del expasión tiene aprox. 0.9K entre 0-5vdc generados por el LTC3900, pero funciona OK
*/
#include <Arduino.h>
//#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include "system.h"
#include "LTC68042.h"
#include "ltc6804.h"
#include "ikey.h"
#include "kb.h"
#include "lcd_bklight.h"
#include "lcd_fxaux.h"
#include "vscroll.h"
#include "rele.h"
#include "digital_input.h"
#include "analog_input.h"
#include "leds.h"
#include "errorsys.h"
#include "boot.h"
#include "ethernet.h"
#include "DS3231.h"
#include "I2C.h"
#include "time.h"
#include "date.h"
#include "rtcc.h"
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"
#include "menu.h"
#include "menu_mainMenu.h"
#include "menu_bkground.h"
#include <SPI.h>
#include "mySPI.h"
#include "sdcard.h"
#include "sysprint.h"
#include "logf_class.h"
#include "logf.h"
#include "piz.h"

mylcd lcd(22, 23, 24, 25, 26, 27);//LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

/*
  TODO:
  1)check cursor posc.
  2)options to assignment in replacement/insert option

*/
void setup()
{
  unsigned long last_millis = millis();

  fuelsensor_inhibit_comm();

  pinMode(WIZNET_CS, OUTPUT);
  pinMode(SDCARD_CS, OUTPUT);
  pinMode(LTC6804_CS, OUTPUT);

  rele_init();
  out_leds_init();
  digital_input_init();

  lcd.begin(LCD_COL, LCD_ROW);
  lcd.setCursor(0, 0);
  digitalWrite(LCD_PIN_BACKLIGHT, LOW);
  pinMode(LCD_PIN_BACKLIGHT, OUTPUT);
  lcd_bklight_ON();//lcd_bklight_OFF();

  Init_I2C_unimaster();
  InitDS3231();

  SPI_deselectAllperipherals();
  SPI.begin();

  if (sdcard_init())
    if (system_set_functionmode())
      if (mysystem.funct_mode == FUNCT_MODE_DEBUG)
        Serial.begin(230400);

  sdcard_runtest_H10();

  eth_init();
  //
  logf_init();
  piz_init();
  //

  fuelsensor_init();//load param from EEPROM
  jt606_init(last_millis);
  fuelsensor_set_comm();

  vscroll.base_offset_reset();

  sysPrint_lcd_serialPrintln_P(1, LCDAN_STR_FORMAT_ALIGN_CENTER, PSTR("Scanning stacks..."));
  while (!ltc6804_job())
  {
    ;
  }

  //
  key_initialization();
  InitTCNT1();
  TIMSK1 = (1 << OCIE1A);
  //

  /*Added test relays 1 set 2020*/
   PinTo1(PORTWxRELE1, PINxRELE1);
   PinTo1(PORTWxRELE2, PINxRELE2);
   PinTo1(PORTWxRELE3, PINxRELE3);
   PinTo1(PORTWxRELE4, PINxRELE4);
   PinTo1(PORTWxRELE5, PINxRELE5);
   /*end test relays*/

   /*Added test leds 1 set 2020*/
   PinTo1(PORTWxLED1, PINxLED1);
   PinTo1(PORTWxLED2, PINxLED2);
   PinTo1(PORTWxLED3, PINxLED3);
   PinTo1(PORTWxLED4, PINxLED4);
   PinTo1(PORTWxLED5, PINxLED5);
   PinTo1(PORTWxLED6, PINxLED6);
   /*end test leds*/
}

void loop()
{
  rtc_job();
  ltc6804_job();
  BOARDd();
  kb_processKeyRead_D();
  menu_detect_changeMenu_changeSubMenu();
  //////////////////////////////////////////////////
  if (menu_getMenu() == MGO_MBKGROUND)//0
  {
    board_event();
    menu_v[MGO_MBKGROUND][submen]();
  }
  else//menu_getMenu() = MGO_MAIN_MENU(-1) or [1,2..8]
  {
    if (menu_getMenu() == MGO_MAIN_MENU)//-1
      mainMenuSummary_job();
    else	//[1,2..8]
      menu_v[men][submen]();
  }
  //////////////////////////////////////////////////
  if (boardD.AchangeInBoard == 1)
    boardD.AchangeInBoard = 0;
  //////////////////////////////////////////////////

  lcd_bklight_job();//display off/on
  analog_input_job();//now in menu input over demand / calibrated via logical exp. menu
  digital_input_job();
  out_leds_job();
  rele_job();

  logf_logExpAssig_man();//-> move interrupt @5ms

  fuelsensor_D();//daemon scheduler
  fuelsensor_job();

  eth_job();
}

ISR(TIMER1_COMPA_vect)
{
  //logf_logExpAssig_man();

#ifdef LCD_BACKLIGHT_TIMMING
  lcd_bklight_timming();
#endif
  kb_job();
}
