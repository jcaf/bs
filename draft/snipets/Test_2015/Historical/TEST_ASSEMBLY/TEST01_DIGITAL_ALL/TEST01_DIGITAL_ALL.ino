void setup() {
  // put your setup code here, to run once:
pinMode(10,OUTPUT);

pinMode(6,OUTPUT);
pinMode(5,OUTPUT);
pinMode(4,OUTPUT);
pinMode(3,OUTPUT);
pinMode(2,OUTPUT);

pinMode(16,OUTPUT);
pinMode(17,OUTPUT);
pinMode(18,OUTPUT);
pinMode(19,OUTPUT);
pinMode(20,OUTPUT);
pinMode(21,OUTPUT);

pinMode(22,OUTPUT);
pinMode(23,OUTPUT);
pinMode(24,OUTPUT);
pinMode(25,OUTPUT);
pinMode(26,OUTPUT);
pinMode(27,OUTPUT);
pinMode(28,OUTPUT);

pinMode(29,OUTPUT);
pinMode(30,OUTPUT);
pinMode(31,OUTPUT);
pinMode(32,OUTPUT);
pinMode(33,OUTPUT);

pinMode(34,OUTPUT);
pinMode(35,OUTPUT);
pinMode(36,OUTPUT);
pinMode(37,OUTPUT);
pinMode(38,OUTPUT);
pinMode(39,OUTPUT);

pinMode(40,OUTPUT);
pinMode(41,OUTPUT);
pinMode(42,OUTPUT);
pinMode(43,OUTPUT);

pinMode(50,OUTPUT);
pinMode(51,OUTPUT);
pinMode(52,OUTPUT);

//pinMode(A0,OUTPUT);
DDRF=0xFF;

//-----------------------------------
digitalWrite(10,LOW);

digitalWrite(6,HIGH);
digitalWrite(5,LOW);
digitalWrite(4,HIGH);
digitalWrite(3,LOW);//power status
digitalWrite(2,HIGH);

digitalWrite(16,HIGH);
digitalWrite(17,LOW);
digitalWrite(18,HIGH);
digitalWrite(19,LOW);
digitalWrite(20,HIGH);
digitalWrite(21,LOW);

digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(25,HIGH);
digitalWrite(26,HIGH);
digitalWrite(27,HIGH);
digitalWrite(28,HIGH);

digitalWrite(29,HIGH);
digitalWrite(30,HIGH);
digitalWrite(31,HIGH);
digitalWrite(32,HIGH);
digitalWrite(33,HIGH);

digitalWrite(34,1);
digitalWrite(35,1);
digitalWrite(36,1);
digitalWrite(37,1);
digitalWrite(38,1);
digitalWrite(39,1);

digitalWrite(40,HIGH);
digitalWrite(41,HIGH);
digitalWrite(42,HIGH);
digitalWrite(43,HIGH);

digitalWrite(50,HIGH);
digitalWrite(51,LOW);
digitalWrite(52,HIGH);


PORTF=0xAA;

}

void loop() {
  // put your main code here, to run repeatedly:

}
