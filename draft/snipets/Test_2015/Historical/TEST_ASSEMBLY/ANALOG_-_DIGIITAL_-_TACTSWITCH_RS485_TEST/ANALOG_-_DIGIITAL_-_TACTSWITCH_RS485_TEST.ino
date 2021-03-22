/*
  Analog input, analog output, serial output

 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.

 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground

 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

// These constants won't change.  They're used to give names
// to the pins used:

//Sensor 0-> en la placa se muestra como AN4
const int analogInPin0 = A0;  // Analog input pin that the potentiometer is attached to

//Sensor 1-> en la placa se muestra como AN3
const int analogInPin1 = A1;  // Analog input pin that the potentiometer is attached to

//Sensor 2-> en la placa se muestra como AN2
const int analogInPin2 = A2;  // Analog input pin that the potentiometer is attached to


//Sensor3 -> en la placa se muestra como AN1
const int analogInPin3 = A3;  // Analog input pin that the potentiometer is attached to
int sensorValue = 0;        // value read from the pot



void setup() {
//digital inputs
pinMode(40,INPUT);
pinMode(41,INPUT);
pinMode(42,INPUT);
pinMode(43,INPUT);

//tact switchsinputs UP DOWN ESCAPE ENTER BK_LCD
pinMode(18,INPUT);//UP TACTSW3
pinMode(19,INPUT);//DOWN TACTSW4
pinMode(20,INPUT);//ESCAPE TACTSW5
pinMode(21,INPUT);//ENTER TACTSW6
pinMode(2,INPUT);//BK_LCD TACTSW7

  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
//
  Serial2.begin(9600);//Test ST485
  pinMode(6,OUTPUT);
  digitalWrite(6,HIGH);//TX
  //while(1){  Serial2.print(0xff);}
}

void loop() {
  //  BUG OVERLAY
  //------------
  //ANALOG CONECTOR -> A1->A4 las entradas estan al reves A4->A1

  // read the analog in value:
  sensorValue = analogRead(analogInPin0);
  Serial.print("sensor0 = " );  Serial.println(sensorValue);
  delay(2);
  
  sensorValue = analogRead(analogInPin1);
  Serial.print("sensor1 = " );  Serial.println(sensorValue);
  delay(2);
  
  sensorValue = analogRead(analogInPin2);
  Serial.print("sensor2 = " );  Serial.println(sensorValue);
  delay(2);
  
  sensorValue = analogRead(analogInPin3);
  Serial.print("sensor3 = " );  Serial.println(sensorValue);
  delay(2);
  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  
  ////////////////////////////////////////
  Serial.print("DIGITAL INPUT 1 = " );  Serial.println(digitalRead(40));
  Serial.print("DIGITAL INPUT 2 = " );  Serial.println(digitalRead(41));
  Serial.print("DIGITAL INPUT 3 = " );  Serial.println(digitalRead(42));
  Serial.print("DIGITAL INPUT 4 = " );  Serial.println(digitalRead(43));
  //HCF40106YM013TR es inversor... 
  Serial.print("TACTSWITCH UP = " );  Serial.println(digitalRead(18));
  Serial.print("TACTSWITCH DOWN = " );  Serial.println(digitalRead(19));
  Serial.print("TACTSWITCH ESCAPE= " );  Serial.println(digitalRead(20));
  Serial.print("TACTSWITCH ENTER= " );  Serial.println(digitalRead(21));
  Serial.print("TACTSWITCH BK_LCD= " );  Serial.println(digitalRead(2));
  
  delay(1000);
}
