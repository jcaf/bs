/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */

void setup() 
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial2.begin(9600);
  
  digitalWrite(6,HIGH);
  pinMode(6,OUTPUT);
  
  delay(100);

  Serial2.print(0x24,HEX);
  Serial2.print(0x21,HEX);
  Serial2.print(0x49,HEX);
  Serial2.print(0x44,HEX);
  Serial2.print(0x30,HEX);
  Serial2.print(0x31,HEX);
  Serial2.print(0x33,HEX);
  Serial2.print(0x33,HEX);
  Serial2.print(0x0D,HEX);
  Serial2.print(0x0A,HEX);
  
  
  //Serial2.println("$!ID0133");
  
 delay(20);
  digitalWrite(6,LOW);
    
}

void loop() { // run over and over
  if (Serial2.available()) 
  {
    Serial.print(Serial2.read());
  }
  
  if (Serial.available()) {
    Serial2.print(Serial.read());
  }
  
}

