#include <LiquidCrystal.h>
//#include <SD.h>
/* ---------------------------------------------------------------------------------------------------- */
//RS/RW=GND/E/DB4/DB5/DB6/DB7
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

/* ---------------------------------------------------------------------------------------------------- */
//External Interrupt
const int button_int_18=5; //(interrupcion "5" esta en pin 18 en Arduino Mega 2560)
const int button_int_19=4; //(interrupcion "4" esta en pin 19 en Arduino Mega 2560)
volatile int selected=1;

/* ---------------------------------------------------------------------------------------------------- */
// Correction factor:
float sensorValue = 0;      
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to

float vout= 5;                             //The voltage between 5V and GRND pins
float conversion= vout/1023;

float correction_1= (12.37/12.24);              // (real/leido). El valor "Leido" se obtiene cuando se le aplica primero un "correction =1". El valor real es el del multimetro.
float correction_2= 1;              // (real/leido). El valor "Leido" se obtiene cuando se le aplica primero un "correction =1". El valor real es el del multimetro.

float automatic_correction_factor_1=1;
float automatic_correction_factor_2=1;
// Voltage Divider:
float resistance_4= 20;                        // 20 Kohm
float resistance_5= 10;                        // 10 Kohm
float resistance_6= 10;                        // 10 Kohm
float resistance_7= 10;                        // 10 Kohm
//float resistance_correction_factor= 1;
//(12.44/11.8);  // Real_value/measured_value
/*Pin40=0 : R4=20K, R5= 10K  Para leer 12VDC */
/*Pin40=1 : R6=10K, R7= 10K  Para leer 2VDC */
/* ---------------------------------------------------------------------------------------------------- */

void setup() 
{
  /* ---------------------------------------------------------------------------------------------------- */  
  //External interrupt
  attachInterrupt(button_int_18, increment, RISING);
  attachInterrupt(button_int_19, decrement, RISING);


  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("TEMPEL GROUP ");
  delay (5000);
  
  //BATTERIES  
  pinMode(28,OUTPUT);
  pinMode(29,OUTPUT);
  pinMode(30,OUTPUT);
  pinMode(31,OUTPUT);

  //CALIBRATION RELAYS 
  pinMode(41,OUTPUT);//uno a la vez
  digitalWrite(41,LOW);//5V  bus
  
  pinMode(42,OUTPUT);
  digitalWrite(42,LOW);//3.3 en el bus
  
  //A0
  pinMode(40,INPUT);//DIGITAL
  
  //4 DIGITAL OUPUTS (RELAYS)
  pinMode(43,OUTPUT);
  pinMode(44,OUTPUT);
  pinMode(45,OUTPUT);
  pinMode(46,OUTPUT);

  //4 DIGITAL OUTPUTS (LEDS)
  pinMode(47,OUTPUT);
  pinMode(48,OUTPUT);
  pinMode(49,OUTPUT);
  pinMode(50,OUTPUT);


  //BATTERIES
  for(int q=28;q<=31;q++)
  {
    digitalWrite(q,LOW);
  }

  //4 DIGITAL OUTPUTS (RELAYS)
  for(int q=43;q<=46;q++)
  {
    digitalWrite(q,LOW);
  }
  //4 DIGITAL OUTPUTS (LEDS)
  for(int q=47;q<=59;q++)
  {
    digitalWrite(q,LOW);
  }


  //BACKLIGHT
  pinMode(17,OUTPUT);
  digitalWrite(17,HIGH);//ON LCD
  
  //ULTRASONIC SENSOR
  #define PIN_TRIG 20
  #define PIN_ECO  21
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECO, INPUT);

  Serial.begin(9600);

}

void loop() 
{
  
  digitalWrite(43,HIGH);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("RED DISPONIBLE");
  
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");

  
  /*------------------------------------------------------------------------------*/
  Serial.print("PIN 40 = " );  
  Serial.println(digitalRead(40));

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("SELECT: PIN 40=");
  lcd.print(digitalRead(40));

  lcd.setCursor(0, 1);
  if(digitalRead(40)==0){
    lcd.print("12 VDC RANGE");
  }
  else
  {
    lcd.print("02 VDC RANGE");  
  }
  
  delay(5000);

  /*------------------------------------------------------------------------------*/
  Serial.println("_Pin 41-Rele 5VDC: ");

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("Rele 5.00: PIN=41");

  digitalWrite(41,HIGH);
  delay(500);
  
  if(digitalRead(40)==0)
  {
    sensorValue = (conversion*correction_1)*((resistance_4+resistance_5)/resistance_5)*analogRead(analogInPin);
  }
  else
  {
    sensorValue = (conversion*correction_2)*((resistance_6+resistance_7)/resistance_7)*analogRead(analogInPin);  
  }
  
  automatic_correction_factor_1=5.0/sensorValue;
  
  Serial.print("__sensor A0 = " );  
  Serial.println(sensorValue);  

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(5, 1);
  lcd.print(sensorValue,2);
  lcd.print(" VDC");

  delay(5000);

  digitalWrite(41,LOW);
  
  /*------------------------------------------------------------------------------*/
  Serial.println("_Pin 42-Rele 3.3VDC: ");

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("Rele 3.30: PIN=42");

  digitalWrite(42,HIGH);
  delay(500);
  
  if(digitalRead(40)==0)
  {
    sensorValue = (conversion*correction_1)*((resistance_4+resistance_5)/resistance_5)*analogRead(analogInPin);
  }
  else
  {
    sensorValue = (conversion*correction_2)*((resistance_6+resistance_7)/resistance_7)*analogRead(analogInPin);  
  }
  
  automatic_correction_factor_2=3.3/sensorValue;
  
  
  Serial.print("__sensor A0 = " );  
  Serial.println(sensorValue);  

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(5, 1);
  lcd.print(sensorValue,2);
  lcd.print(" VDC");
  
  delay(5000);

  digitalWrite(42,LOW);
  
  Serial.println("");
  /*------------------------------------------------------------------------------*/
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("f1(5.0)=");
  lcd.print(automatic_correction_factor_1,6);

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("f2(3.3)=");  
  lcd.print(automatic_correction_factor_2,6);
  
  delay(5000);
  /*------------------------------------------------------------------------------*/
  

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  lcd.print("                ");

  lcd.setCursor(0, 1);
  lcd.print("TEST TIME: ");
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");

  //BATERIAS
  
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("BATERIAS: PIN=");
  
  float total=0;
  for(int i=28;i<=31;i++)
  {
    delay(500);
    lcd.setCursor(14, 0);
    lcd.print(i);
    digitalWrite(i,HIGH);
    
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("CELL-");
    lcd.print(i);
    lcd.print("=");
    delay(1000);
    
      
    if(digitalRead(40)==0)
    {
      sensorValue = (conversion*correction_1)*((resistance_4+resistance_5)/resistance_5)*analogRead(analogInPin);
    }
    else
    {
      sensorValue = (conversion*correction_2)*((resistance_6+resistance_7)/resistance_7)*analogRead(analogInPin);  
    }
  
    
    lcd.print(sensorValue,2);
    lcd.print(" V");
    
    digitalWrite(i,LOW);
    delay(2000);
    
    total=total+sensorValue;

  }
    lcd.setCursor(0, 1);
    lcd.print("                ");
    
    lcd.setCursor(0, 1);
    lcd.print("Total= ");
    lcd.print(total);
    lcd.print(" V");
    
    delay(2000);
    lcd.setCursor(0, 1);
    lcd.print("                ");
  
  /*------------------------------------------------------------------------------*/
  //DIGITAL OUTPUTS (RELAYS)
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("RELAYS:   PIN=");

  //El PIN 43 esta iniciando el programa

  for(int k=44;k<=46;k++)
  {
    delay(500);
    lcd.setCursor(14, 0);
    lcd.print(k);
    digitalWrite(k,HIGH);
    delay(500);
    digitalWrite(k,LOW);
  }
  
  //lcd.setCursor(14, 0);
  //lcd.print(43);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("FALLA DE RED");
  digitalWrite(43,LOW);
  delay(60000);
  digitalWrite(43,HIGH);
  lcd.setCursor(0, 1);
  lcd.print("                ");

  /*------------------------------------------------------------------------------*/
  //DIGITAL OUTPUTS (LEDS)
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("LEDS:    PIN=");

  for(int m=47;m<=50;m++)
  {
    delay(500);
    lcd.setCursor(13, 0);
    lcd.print(m);
    digitalWrite(m,HIGH);
    delay(500);
    digitalWrite(m,LOW);
  }
  
  lcd.setCursor(13, 0);
  lcd.print("ALL");

   for(int m=47;m<=50;m++)
  {
    digitalWrite(m,HIGH);
  }
  
  delay(1000);
  
   for(int m=47;m<=50;m++)
  {
    digitalWrite(m,LOW);
  }
  
  
  ultrasonic();
  

}

void ultrasonic()
{  long duracion, distancia;  // Variables
  
  /* Hacer el disparo */
  digitalWrite(PIN_TRIG, LOW);  
  delayMicroseconds(2); 
  digitalWrite(PIN_TRIG, HIGH);  // Flanco ascendente
  delayMicroseconds(10);        // Duracion del pulso
  digitalWrite(PIN_TRIG, LOW);  // Flanco descendente
  
  /* Recepcion del eco de respuesta */
  duracion = pulseIn(PIN_ECO, HIGH);
  
  /* Calculo de la distancia efectiva */
  distancia = (duracion/2) / 29;

  /* Imprimir resultados a la terminal serial */
  if (distancia >= 4000 || distancia <= 0){
    Serial.println("Fuera de rango");
  }
  else {
    Serial.print("Longitud: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }


  
  // Retardo para disminuir la frecuencia de las lecturas
  delay(50);  
  
  Serial.println();

}

/* ---------------------------------------------------------------------------------------------------- */
void increment()
{
/*
    previousMillis = millis();
    previous_backlight_Millis = millis();
    backlight_status=1;
    
    if(selected < numCells)
    {
      selected =selected + 1;
    }
    else 
    {
      selected = 1;
    }
    lcd_print_data();
*/    
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("INTERRUPCION INC");

  //return;
}

/* ---------------------------------------------------------------------------------------------------- */
void decrement()
{
/*
    previousMillis = millis();
    previous_backlight_Millis = millis();
    backlight_status=1;

    if(selected > 1)
    {
      selected =selected - 1;     
    }
    else 
    {
      selected = numCells;
    }
    lcd_print_data();
*/    
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("INTERRUPCION DEC");

  //return;
    
}

